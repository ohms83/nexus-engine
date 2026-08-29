#include "scene/renderer/DummySceneRenderer.h"

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

#include "scene/Camera.h"
#include "scene/component/CameraComponent.h"
#include "scene/component/LightComponent.h"
#include "scene/component/ModelComponent.h"
#include "scene/component/SceneNodeComponent.h"
#include "scene/component/TransformComponent.h"

USING_NAMESPACE_NXS;

DummySceneRenderer::DummySceneRenderer()
{
    RegisterRenderPass(OpaquePass);
}

void DummySceneRenderer::Render(RenderSystem& renderSystem, const Scene& scene)
{
#if 0
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

            const auto mvpMtx = projection * viewMtx * modelMtx;
            for (auto mesh : model->GetMeshes())
            {
                if (!IsSphereInside(viewFrustum, mesh->GetSphere(), modelMtx, scale.value)) continue;
                commands.emplace_back(CreateRenderCommand(mesh, std::move(modelMtx), mvpMtx));
            }
        }

        {
            std::ranges::sort(commands, [](const RenderCommand& a, const RenderCommand& b) {
                return a.sortKey < b.sortKey;
            });
        }
        {
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
                        if (!cmd.material) continue;
                        if (!pass.IsPassFiltered(cmd)) continue;

                        Ref<GpuProgram> gpuProgram = pass.pipelineState.globalShader;
                        if (!gpuProgram) {
                            gpuProgram = cmd.material->Use();
                        }
                        if (gpuProgram != usingProgram)
                        {
                            if (!gpuProgram->IsBinding()) gpuProgram->Bind();

                            gpuProgram->SetUniformVector("_CameraPos", cameraPos.value);
                            gpuProgram->SetUniformMatrix("_View", viewMtx, false);
                            gpuProgram->SetUniformMatrix("_Projection", projection, false);

                            SetAmbientLightParams(gpuProgram, registry);
                            SetDirectLightParams(gpuProgram, registry);
                            SetPointLightParams(gpuProgram, registry);
                            usingProgram = gpuProgram;
                        }
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
    }
#endif
}