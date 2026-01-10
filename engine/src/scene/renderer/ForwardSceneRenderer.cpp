#include "scene/renderer/ForwardSceneRenderer.h"

#include <format>
#include <algorithm>
#include <cstring>

#include "core/Logger.h"
#include "graphics/debug/Gizmos.h"
#include "graphics/RenderSystem.h"
#include "graphics/RenderCommand.h"
#include "graphics/RenderTarget.h"
#include "graphics/RenderCommandBatcher.h"
#include "graphics/RenderCommand.h"
#include "graphics/RenderGraph.h"
#include "graphics/RenderPass.h"
#include "geom/Frustum.h"
#include "ecs/Ecs.h"
#include "math/Math.h"
#include "math/Matrix.h"
#include "memory/OwningBuffer.h"
#include "scene/component/CameraComponent.h"
#include "scene/component/LightComponent.h"
#include "scene/component/ModelComponent.h"
#include "scene/component/SceneNodeComponent.h"
#include "scene/component/TransformComponent.h"

#include "Remotery.h"

USING_NAMESPACE_NXS;

ForwardSceneRenderer::ForwardSceneRenderer(const RenderSystem& renderSystem)
{
    RegisterRenderPass(DepthPrepass);
    RegisterRenderPass(OpaquePass);
    RegisterRenderPass(AlphaPass);
    // Overlay pass is still buggy. Temporarily disable it.
    // RegisterRenderPass(OverlayPass);
}

void ForwardSceneRenderer::Render(RenderSystem& renderSystem, const entt::registry& registry)
{
    rmt_ScopedCPUSample(SceneRenderer_Render, 0);

    std::vector<RenderCommand> commands;
    commands.reserve(1024);

    // Storing mesh's model matrix for the rendering phase.
    std::vector<glm::mat4> modelMatrices;
    auto renderInterface = renderSystem.GetRenderInterface();

    // ReSharper disable once CppTooWideScopeInitStatement
    const auto cameraView = registry.view<SceneNodeComponent, CameraComponent, PositionComponent, OrientationComponent>();
    for (const auto& [cameraEntity, cameraNode, camera, cameraPos, cameraOrient] : cameraView.each())
    {
        if (!cameraNode.active) continue;

        glm::mat4 viewMtx = Matrix::CreateViewMatrix(cameraPos.value, cameraOrient.quat);
        glm::mat4 projection;
        if (camera.projectionType == ProjectionType::Perspective) {
            projection = glm::perspective(glm::radians(camera.fov), camera.width / camera.height, camera.nearZ, camera.farZ);
        }
        else {
            projection = glm::ortho(-camera.width/2, camera.width/2, -camera.height/2, camera.height/2, camera.nearZ, camera.farZ);
        }

        const auto viewProjMtx = projection * viewMtx;
        const auto viewFrustum = camera.GetViewFrustum(cameraPos.value, cameraOrient.quat);
        for (const auto view = registry.view<SceneNodeComponent, ModelComponent, PositionComponent, OrientationComponent, ScaleComponent>(); const auto& [entity, sceneNode, modelComp, position, orient, scale] : view.each())
        {
            auto model = modelComp.model;
            if (!sceneNode.active || !model) continue;

            glm::mat4 modelMtx = Matrix::CreateModelMatrix(position.value, orient.quat, scale.value);

            if (!IsSphereInside(viewFrustum, model->GetBoundingSphere(), modelMtx, scale.value)) continue;

            rmt_BeginCPUSample(SceneRenderer_CreateSortList, 0)
            const auto mvpMtx = projection * viewMtx * modelMtx;
            for (auto mesh : model->GetMeshes())
            {
                if (!IsSphereInside(viewFrustum, mesh->GetSphere(), modelMtx, scale.value)) continue;
                commands.emplace_back(CreateRenderCommand(mesh, std::move(modelMtx), mvpMtx));
            }
            rmt_EndCPUSample();
        }

        {
            rmt_ScopedCPUSample(SceneRenderer_SortMeshes, 0)
            std::ranges::sort(commands, [](const RenderCommand& a, const RenderCommand& b) {
                return a.sortKey < b.sortKey;
            });
        }
        {
            rmt_ScopedCPUSample(SceneRenderer_RenderMeshes, 0)

            if (!m_renderPasses.empty())
            {
                auto ordered = nxs::RenderGraph::Build(m_renderPasses);
                for (const auto& pass : ordered)
                {
                    if (!pass.enabled) continue;
                    if (pass.targetType == RenderTargetType::Offscreen && !pass.offscreenTargetName.empty()) {
                        if (const auto it = m_renderTargets.find(pass.offscreenTargetName); it != m_renderTargets.end() && it->second) {
                            it->second->Bind(renderSystem);
                        }
                    }
                    pass.Begin(renderSystem);

                    Ref<GpuProgram> usingProgram = nullptr;
                    // Batch commands in-place to reduce allocations and combine adjacents.
                    RenderCommandBatcher::Batch(commands);
                    const auto &batched = commands;
                    for (const auto& cmd : batched)
                    {
                        auto material = cmd.material;
                        if (!material) continue;
                        if (!pass.MatchesMaterial(*material)) continue;

                        Ref<GpuProgram> gpuProgram = pass.pipelineState.globalShader ? pass.pipelineState.globalShader : cmd.gpuProgram;
                        if (gpuProgram != usingProgram)
                        {
                            if (!pass.pipelineState.globalShader)
                            {
                                material->Use();
                            }
                            else
                            {
                                if (!gpuProgram->IsBinding()) gpuProgram->Bind();
                            }

                            gpuProgram->SetUniformVector("_CameraPos", cameraPos.value);
                            gpuProgram->SetUniformMatrix("_View", viewMtx, false);
                            gpuProgram->SetUniformMatrix("_Projection", projection, false);

                            SetAmbientLightParams(gpuProgram, registry);
                            SetDirectLightParams(gpuProgram, registry);
                            SetPointLightParams(gpuProgram, registry);
                            usingProgram = gpuProgram;
                        }

                        if (!cmd.instanceModels.empty())
                        {
                            // Build an OwningBuffer holding all model matrices
                            const auto instanceCount = static_cast<uint32>(cmd.instanceModels.size());
                            const size_t bytes = instanceCount * sizeof(glm::mat4);
                            uint8_t* mem = new uint8_t[bytes];
                            // Copy matrices
                            for (uint32 i = 0; i < instanceCount; ++i)
                            {
                                std::memcpy(&mem[i * sizeof(glm::mat4)], &cmd.instanceModels[i], sizeof(glm::mat4));
                            }
                            Ref<IBuffer> instanceBuffer = std::make_shared<OwningBuffer>(mem, bytes);

                            // Create instance attributes for mat4 as 4 vec4 attributes (layout locations 10..13)
                            std::vector<VertexAttribute> attrs;
                            for (int i = 0; i < 4; ++i)
                            {
                                VertexAttribute a;
                                a.type = VertexAttribute::Type::TexCoord0; // type is unused if attribIndex set
                                a.dataType = DataType::Float;
                                a.numElements = 4;
                                a.attribIndex = 10 + i; // locations 10..13
                                a.divisor = 1;
                                attrs.push_back(a);
                            }

                            cmd.vertexBuffer->AttachInstanceStream(std::static_pointer_cast<IBuffer>(instanceBuffer), attrs, BufferUsage::StaticDraw);
                            renderSystem.DrawIndexedInstanced(cmd.indexBuffer, instanceCount);
                            cmd.vertexBuffer->DetachInstanceStreams();
                            // free temporary allocation (OwningBuffer::Take took ownership)
                            // OwningBuffer destructor will free it
                        }
                        else
                        {
                            gpuProgram->SetUniformMatrix("_Model", cmd.modelMatrix, false);
                            cmd.vertexBuffer->Bind();
                            cmd.indexBuffer->Bind();
                            renderSystem.DrawIndexed(cmd.indexBuffer);
                        }
                    }

                    pass.End(renderSystem);
                    if (pass.targetType == RenderTargetType::Offscreen && !pass.offscreenTargetName.empty()) {
                        if (const auto it = m_renderTargets.find(pass.offscreenTargetName); it != m_renderTargets.end() && it->second) {
                            it->second->Unbind(renderSystem);
                        }
                    }
                }
            }
        }

        for (const auto view = registry.view<SceneNodeComponent, MeshComponent>(); const auto& [entity, sceneNode, meshComp] : view.each())
        {
            auto mesh = meshComp.mesh;
            if (meshComp.showBoundingBox)
            {
                const auto& box = mesh->GetBox();
                Gizmos::DrawOutlineBox(renderSystem, box.center, box.extent);
            }
            if (meshComp.showBoundingSphere)
            {
                const auto& sphere = mesh->GetSphere();
                Gizmos::DrawOutlineSphere(renderSystem, sphere.center, sphere.radius);
            }
        }
        // TODO: Perform drawing during the overlay render pass
        Gizmos::ProcessDraw(renderSystem, projection * viewMtx);
        // Only render from the first active camera POV.
        // TODO: Support render to offscreen targets from multiple cameras.
        break;
    }
}
