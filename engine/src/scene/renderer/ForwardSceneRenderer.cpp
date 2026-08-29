#include "scene/renderer/ForwardSceneRenderer.h"

#include <format>
#include <algorithm>
#include <cstring>

#include "nexus/debug/Logger.h"
#include "graphics/debug/Gizmos.h"
#include "graphics/RenderSystem.h"
#include "graphics/RenderCommand.h"
#include "graphics/RenderTarget.h"
#include "graphics/RenderCommandBatcher.h"
#include "graphics/RenderCommand.h"
#include "graphics/RenderGraph.h"
#include "graphics/RenderPass.h"
#include "nexus/geom/Frustum.h"
#include "ecs/Ecs.h"
#include "nexus/math/Math.h"
#include "nexus/math/Matrix.h"
#include "nexus/memory/OwningBuffer.h"
#include "scene/Scene.h"
#include "scene/component/CameraComponent.h"
#include "scene/component/LightComponent.h"
#include "scene/component/MeshComponent.h"
#include "scene/component/SceneNodeComponent.h"
#include "scene/component/TransformComponent.h"

#include "Remotery.h"

USING_NAMESPACE_NXS;

ForwardSceneRenderer::ForwardSceneRenderer(const RenderSystem& renderSystem)
{
    RegisterRenderPass(OpaquePass);
    RegisterRenderPass(AlphaPass);
    RegisterRenderPass(OverlayPass);
}

void ForwardSceneRenderer::Render(RenderSystem& renderSystem, const Scene& scene)
{
    rmt_ScopedCPUSample(SceneRenderer_Render, 0);

    std::vector<RenderCommand> commands;
    commands.reserve(1024);

    const auto& registry = *scene.GetRegistry();
    auto renderInterface = renderSystem.GetRenderInterface();

    const auto cameraView = registry.view<SceneNodeComponent, CameraComponent, PositionComponent, OrientationComponent>();
    for (const auto& [cameraEntity, cameraNode, camera, cameraPos, cameraOrient] : cameraView.each())
    {
        if (!cameraNode.active) continue;

        glm::mat4 viewMtx = Matrix::CreateViewMatrix(cameraPos.value, cameraOrient.quat);
        glm::mat4 projection;
        if (camera.projectionType == ProjectionType::Perspective) {
            NXS_ASSERT(!Math::AlmostZero(camera.nearZ) && !Math::AlmostZero(camera.farZ) && !Math::AlmostZero(camera.height));
            projection = glm::perspective(glm::radians(camera.fov), camera.width / camera.height, camera.nearZ, camera.farZ);
        }
        else {
            NXS_ASSERT(!Math::AlmostZero(camera.nearZ));
            projection = glm::ortho(-camera.width/2, camera.width/2, -camera.height/2, camera.height/2, camera.nearZ, camera.farZ);
        }

        const auto viewProjMtx = projection * viewMtx;
        const auto viewFrustum = camera.GetViewFrustum(cameraPos.value, cameraOrient.quat);
        for (const auto view = registry.view<SceneNodeComponent, MeshComponent, PositionComponent, OrientationComponent, ScaleComponent>(); const auto& [entity, sceneNode, meshComp, position, orient, scale] : view.each())
        {
            if (!sceneNode.active) continue;

            glm::mat4 modelMtx = Matrix::CreateModelMatrix(position.value, orient.quat, scale.value);

            rmt_BeginCPUSample(SceneRenderer_CreateSortList, 0)
            const auto mvpMtx = projection * viewMtx * modelMtx;

            if (auto mesh = meshComp.GetMesh(); mesh != nullptr)
            {
                if (!IsSphereInside(viewFrustum, mesh->GetSphere(), modelMtx, scale.value)) continue;
                commands.emplace_back(CreateRenderCommand(mesh, std::move(modelMtx), mvpMtx));

                if (meshComp.showBoundingBox)
                {
                    const auto& box = mesh->GetBox();
                    Gizmos::DrawOutlineBox(renderSystem, box.center, box.extent, modelMtx);
                }
                if (meshComp.showBoundingSphere)
                {
                    const auto& sphere = mesh->GetSphere();
                    Gizmos::DrawOutlineSphere(renderSystem, sphere.center, sphere.radius, modelMtx);
                }
            }
            rmt_EndCPUSample();
        }

        Gizmos::CreateRenderCommands(commands, renderSystem);

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
                    for (const auto& cmd : commands)
                    {
                        if (!cmd.material) continue;
                        if (!pass.IsPassFiltered(cmd)) continue;

                        Ref<GpuProgram> gpuProgram = pass.pipelineState.globalShader;
                        if (!gpuProgram) {
                            gpuProgram = cmd.material->Use();
                        }
                        if (gpuProgram != usingProgram)
                        {
                            if (!gpuProgram->IsBinding()) gpuProgram->Bind();

                            SetCameraUniforms(gpuProgram, cameraPos.value, viewMtx, projection);
                            SetAmbientLightParams(gpuProgram, registry);
                            SetDirectLightParams(gpuProgram, registry);
                            SetPointLightParams(gpuProgram, registry);
                            usingProgram = gpuProgram;
                        }

                        if (!RenderInstancedMesh(renderSystem, cmd, gpuProgram))
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

        // Only render from the first active camera POV.
        // TODO: Support render to offscreen targets from multiple cameras.
        break;
    }
}
