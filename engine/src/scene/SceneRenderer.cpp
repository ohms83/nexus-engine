//
// Created by nutta on 7/25/2025.
//
#include "scene/SceneRenderer.h"

#include <format>
#include <algorithm>

#include "core/Logger.h"
#include "graphics/debug/Gizmos.h"
#include "graphics/RenderSystem.h"
#include "graphics/RenderCommand.h"
#include "graphics/RenderTarget.h"
#include "graphics/RenderCommandBatcher.h"
#include "graphics/RenderCommand.h"
#include "graphics/RenderGraph.h"
#include "geom/Frustum.h"
#include "ecs/Ecs.h"
#include "math/Math.h"
#include "math/Matrix.h"
#include "scene/component/CameraComponent.h"
#include "scene/component/LightComponent.h"
#include "scene/component/ModelComponent.h"
#include "scene/component/SceneNodeComponent.h"
#include "scene/component/TransformComponent.h"

#include "Remotery.h"

// Shader sources
static const char* s_depthVertexShader = R"(
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 _Model;
uniform mat4 _View;
uniform mat4 _Projection;

void main()
{
    gl_Position = _Projection * _View * _Model * vec4(aPos, 1.0);
    gl_Position += 0.1;
}
)";

const char* s_depthFragmentShader = R"(
#version 330 core
void main()
{
    // Do nothing. This shader outputs no color (gl_FragColor is not written), 
    // but the depth value determined by gl_Position is still written to the depth buffer.
}
)";

USING_NAMESPACE_NXS;

DEFINE_LOG(ForwardSceneRenderer);

namespace
{
    // Render command-based sorting/batching
}

static void SetAmbientLightParams(Ref<GpuProgram> gpuProgram, const entt::registry& registry)
{
    rmt_ScopedCPUSample(SceneRenderer_SetAmbientLightParams, 0);
    const auto ambientLightEnt = registry.view<AmbientLightComponent>().front();
    const auto color = ambientLightEnt == entt::null ? Color3F::Grey : registry.get<AmbientLightComponent>(ambientLightEnt).color;
    gpuProgram->SetUniformVector("_AmbientLight", color);
}

static void SetDirectLightParams(Ref<GpuProgram> gpuProgram, const entt::registry& registry)
{
    rmt_ScopedCPUSample(SceneRenderer_SetDirectLightParams, 0);
    int32 numLight = 0;
    for (const auto view = registry.view<SceneNodeComponent, DirectLightComponent>(); const auto& [entity, node, light] : view.each())
    {
        if (!node.active) continue;

        // TODO: Cache the uniform names.
        const auto uniformLocation = std::format("_DirectLights[{}]", numLight);
        const auto uniformLocationColor = std::format("{}.properties.color", uniformLocation);
        const auto uniformLocationDiffuse = std::format("{}.properties.diffuseIntensity", uniformLocation);
        const auto uniformLocationSpecular = std::format("{}.properties.specularIntensity", uniformLocation);
        const auto uniformLocationCutoff = std::format("{}.properties.cutoff", uniformLocation);
        const auto uniformLocationDirection = std::format("{}.direction", uniformLocation);

        gpuProgram->SetUniformVector(uniformLocationColor, light.properties.color);
        gpuProgram->SetUniformVector(uniformLocationDirection, light.direction);
        gpuProgram->SetUniformFloat(uniformLocationDiffuse, light.properties.diffuseIntensity);
        gpuProgram->SetUniformFloat(uniformLocationSpecular, light.properties.specularIntensity);
        gpuProgram->SetUniformFloat(uniformLocationCutoff, light.properties.cutoffRange);

        numLight++;
    }
    gpuProgram->SetUniformInt("_NumDirectLight", numLight);
}

static void SetPointLightParams(Ref<GpuProgram> gpuProgram, const entt::registry& registry)
{
    rmt_ScopedCPUSample(SceneRenderer_SetPointLightParams, 0);
    int32 numLight = 0;
    for (const auto view = registry.view<SceneNodeComponent, PointLightComponent, PositionComponent>(); const auto& [entity, node, light, position] : view.each())
    {
        if (!node.active) continue;

        rmt_BeginCPUSample(CreateUniformNames, 0);
        // TODO: Cache the uniform names.
        const auto uniformLocation = std::format("_PointLights[{}]", numLight);
        const auto uniformLocationColor = std::format("{}.properties.color", uniformLocation);
        const auto uniformLocationDiffuse = std::format("{}.properties.diffuseIntensity", uniformLocation);
        const auto uniformLocationSpecular = std::format("{}.properties.specularIntensity", uniformLocation);
        const auto uniformLocationCutOff = std::format("{}.cutoff", uniformLocation);
        const auto uniformLocationPosition = std::format("{}.position", uniformLocation);
        const auto uniformLocationConst = std::format("{}.constant", uniformLocation);
        const auto uniformLocationLinear = std::format("{}.linear", uniformLocation);
        const auto uniformLocationQuad = std::format("{}.quadratic", uniformLocation);
        rmt_EndCPUSample();

        rmt_BeginCPUSample(EmplaceUniformValues, 0);
        gpuProgram->SetUniformVector(uniformLocationPosition, position.value);
        gpuProgram->SetUniformVector(uniformLocationColor, light.properties.color);
        gpuProgram->SetUniformFloat(uniformLocationDiffuse, light.properties.diffuseIntensity);
        gpuProgram->SetUniformFloat(uniformLocationSpecular, light.properties.specularIntensity);
        gpuProgram->SetUniformFloat(uniformLocationCutOff, light.properties.cutoffRange);
        gpuProgram->SetUniformFloat(uniformLocationConst, light.constant);
        gpuProgram->SetUniformFloat(uniformLocationLinear, light.linear);
        gpuProgram->SetUniformFloat(uniformLocationQuad, light.quadratic);
        rmt_EndCPUSample();

        numLight++;
    }
    gpuProgram->SetUniformInt("_NumPointLight", numLight);
}

static bool IsSphereInside(const Frustum& viewFustrum, const Sphere& sphere, glm::mat4 modelMtx, const glm::vec3& scale)
{
    const glm::vec3 pos = modelMtx * glm::vec4(sphere.center, 1);
    const float maxScale = std::max(std::max(scale.x, scale.y), scale.z);
    const float scaledRadius = sphere.radius * maxScale;
    return viewFustrum.IsSphereInside(pos, scaledRadius);
}

ForwardSceneRenderer::ForwardSceneRenderer(const RenderSystem& renderSystem)
{
    Hasher hasher;
    // Depth shader used by depth-only prepass
    const auto name = "_DepthShader";
    Ref<Shader> depthShader = std::make_shared<Shader>(name, hasher.Hash32(name));
    depthShader->CompileFromSource(*renderSystem.GetRenderInterface(), s_depthVertexShader, s_depthFragmentShader);

    // Default passes
    // Depth prepass
    RenderPass depthPass;
    depthPass.enabled = false; // Temporarily disable depth prepass
    depthPass.clearFlags = ClearFlags::Depth;
    depthPass.SetName("DepthPrepass").SetPriority(RENDER_PASS_DEPTH_FILL);
    depthPass.pipelineState.depthWrite = true;
    depthPass.pipelineState.depthTest = true;
    depthPass.SetGlobalShader(depthShader);
    depthPass.filter = [](const Material& m) { return m.blendMode == BlendMode::None; };
    RegisterRenderPass(depthPass);

    // Opaque pass
    RenderPass opaquePass;
    opaquePass.SetName("Opaque").SetPriority(RENDER_PASS_OPAQUE);
    opaquePass.clearFlags = ClearFlags::Color | ClearFlags::Depth;
    opaquePass.pipelineState.depthWrite = true;
    opaquePass.pipelineState.depthTest = true;
    opaquePass.filter = [](const Material& m) { return m.blendMode == BlendMode::None; };
    RegisterRenderPass(opaquePass);

    // Alpha/translucent pass
    RenderPass alphaPass;
    // Temporarily disable alpha pass. There's a bug in the ApplyPipelineState causing issues.
    alphaPass.enabled = false;
    alphaPass.clearFlags = ClearFlags::None;
    alphaPass.SetName("Alpha").SetPriority(RENDER_PASS_ALPHA);
    alphaPass.pipelineState.depthWrite = false;
    alphaPass.pipelineState.depthTest = true;
    alphaPass.filter = [](const Material& m) { return m.blendMode != BlendMode::None; };
    RegisterRenderPass(alphaPass);

    // Overlay pass (for UI / always-on-top)
    RenderPass overlayPass;
    // Temporarily disable overlay pass. There's a bug in the ApplyPipelineState causing issues.
    overlayPass.enabled = false;
    overlayPass.clearFlags = ClearFlags::None;
    overlayPass.SetName("Overlay").SetPriority(RENDER_PASS_OVERLAY);
    // default pipeline state for overlay, usually draws last
    overlayPass.filter = [](const Material& m) {
        // Temporary: no overlay materials yet
        return false;
    };
    RegisterRenderPass(overlayPass);
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

            const glm::mat4& modelMtx = modelMatrices.emplace_back(Matrix::CreateModelMatrix(position.value, orient.quat, scale.value));

            if (!IsSphereInside(viewFrustum, model->GetBoundingSphere(), modelMtx, scale.value)) continue;

            rmt_BeginCPUSample(SceneRenderer_CreateSortList, 0)
            const auto mvpMtx = projection * viewMtx * modelMtx;
            for (auto mesh : model->GetMeshes())
            {
                if (!IsSphereInside(viewFrustum, mesh->GetSphere(), modelMtx, scale.value)) continue;

                const auto material = mesh->GetMaterial();
                const auto meshSphere = mesh->GetSphere();
                const auto pos = mvpMtx * glm::vec4(meshSphere.center, 1);
                const auto clipZ = pos.z / pos.w;
                const bool translucent = (material->blendMode != BlendMode::None);
                const uint32_t materialId = 0x7FFFFFFF & material->GetId();
                const float depthN = clipZ; // normalized in -1..1
                const float depthNormalized = (depthN + 1.0f) * 0.5f;
                RenderCommand cmd;
                cmd.vertexBuffer = mesh->GetVertexBuffer();
                cmd.indexBuffer = mesh->GetIndexBuffer();
                cmd.indexCount = mesh->GetIndexBuffer()->GetNumIndexDraw();
                cmd.indexOffset = 0;
                cmd.vertexOffset = 0;
                cmd.modelMatrix = &modelMtx;
                cmd.bounds = mesh->GetSphere();
                cmd.layerMask = 0xFFFFFFFFu;
                cmd.material = material;
                cmd.gpuProgram = material->GetShader()->GetGpuProgram();
                cmd.SetSortKey(translucent, materialId, depthNormalized);
                commands.emplace_back(std::move(cmd));
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
            // LOG_DEBUG(LogBasicSceneRenderer, std::format("Begin Draw..."));
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
                    const auto batched = RenderCommandBatcher::Batch(commands);
                    for (const auto& cmd : batched)
                    {
                        auto material = cmd.material;
                        if (!material) continue;
                        if (!pass.MatchesMaterial(*material)) continue;

                        Ref<GpuProgram> gpuProgram = pass.globalShader ? pass.globalShader->GetGpuProgram() : cmd.gpuProgram;
                        if (gpuProgram != usingProgram)
                        {
                            if (!pass.globalShader)
                            {
                                material->Use();
                            }
                            else
                            {
                                if (!gpuProgram->IsBinding()) gpuProgram->Bind();
                            }

                            gpuProgram->SetUniformMatrix("_View", viewMtx, false);
                            gpuProgram->SetUniformMatrix("_Projection", projection, false);

                            SetAmbientLightParams(gpuProgram, registry);
                            SetDirectLightParams(gpuProgram, registry);
                            SetPointLightParams(gpuProgram, registry);
                            usingProgram = gpuProgram;
                        }

                        if (!cmd.instanceModels.empty())
                        {
                            for (const auto mtxPtr : cmd.instanceModels)
                            {
                                gpuProgram->SetUniformMatrix("_Model", *mtxPtr, false);
                                cmd.vertexBuffer->Bind();
                                cmd.indexBuffer->Bind();
                                renderSystem.DrawIndexed(cmd.indexBuffer);
                            }
                        }
                        else
                        {
                            if (cmd.modelMatrix) gpuProgram->SetUniformMatrix("_Model", *cmd.modelMatrix, false);
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
            else
            {
                Material* usingMaterial = nullptr;
                const auto batched = RenderCommandBatcher::Batch(commands);
                for (const auto& cmd : batched)
                {
                    auto material = cmd.material;
                    if (material.get() != usingMaterial)
                    {
                        material->Use();
                        usingMaterial = material.get();

                        renderInterface->SetDepthFunction(material->depthFunction);
                        auto gpuProgram = material->GetShader()->GetGpuProgram();
                        gpuProgram->SetUniformMatrix("_View", viewMtx, false);
                        gpuProgram->SetUniformMatrix("_Projection", projection, false);

                        SetAmbientLightParams(gpuProgram, registry);
                        SetDirectLightParams(gpuProgram, registry);
                        SetPointLightParams(gpuProgram, registry);
                    }

                    auto gpuProgram = material->GetShader()->GetGpuProgram();
                    if (!cmd.instanceModels.empty())
                    {
                        for (const auto mtxPtr : cmd.instanceModels)
                        {
                            gpuProgram->SetUniformMatrix("_Model", *mtxPtr, false);
                            cmd.vertexBuffer->Bind();
                            cmd.indexBuffer->Bind();
                            renderSystem.DrawIndexed(cmd.indexBuffer);
                        }
                    }
                    else
                    {
                        if (cmd.modelMatrix) gpuProgram->SetUniformMatrix("_Model", *cmd.modelMatrix, false);
                        cmd.vertexBuffer->Bind();
                        cmd.indexBuffer->Bind();
                        renderSystem.DrawIndexed(cmd.indexBuffer);
                    }
                }
            }

            // LOG_DEBUG(LogBasicSceneRenderer, std::format("End Draw"));
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
        Gizmos::ProcessDraw(renderSystem, projection * viewMtx);
        // Only render from the first active camera POV.
        break;
    }
}
