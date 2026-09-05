//
// Created by nutta on 7/25/2025.
//
#include "nexus/scene/renderer/SceneRenderer.h"

#include <format>
#include <algorithm>
#include <cstring>

#include "nexus/debug/Logger.h"
#include "nexus/graphics/debug/Gizmos.h"
#include "nexus/graphics/RenderSystem.h"
#include "nexus/graphics/RenderCommand.h"
#include "nexus/graphics/RenderTarget.h"
#include "nexus/graphics/RenderCommandBatcher.h"
#include "nexus/graphics/RenderCommand.h"
#include "nexus/geom/Frustum.h"
#include "nexus/ecs/Ecs.h"
#include "nexus/math/Math.h"
#include "nexus/math/Matrix.h"
#include "nexus/memory/OwningBuffer.h"
#include "nexus/scene/RenderGraph.h"
#include "nexus/scene/Scene.h"
#include "nexus/scene/component/CameraComponent.h"
#include "nexus/scene/component/LightComponent.h"
#include "nexus/scene/component/ModelComponent.h"
#include "nexus/scene/component/SceneNodeComponent.h"
#include "nexus/scene/component/TransformComponent.h"
#include "nexus/scene/component/MeshComponent.h"

#include "Remotery.h"

USING_NAMESPACE_NXS;

DEFINE_LOG(SceneRenderer);

RenderCommand SceneRenderer::CreateRenderCommand(Ref<const Mesh> mesh, glm::mat4&& modelMtx, const glm::mat4& mvpMtx)
{
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
    cmd.modelMatrix = modelMtx;
    cmd.bounds = mesh->GetSphere();
    cmd.layerMask = 0xFFFFFFFFu;
    cmd.material = material;
    cmd.SetSortKey(translucent, materialId, depthNormalized);
    return cmd;
}

bool SceneRenderer::RenderInstancedMesh(RenderSystem& renderSystem, const RenderCommand& command, Ref<GpuProgram> gpuProgram)
{
    rmt_ScopedCPUSample(SceneRenderer_RenderInstancedMesh, 0);
    if (command.instanceModels.empty()) return false;

    // Build an OwningBuffer holding all model matrices
    const auto instanceCount = static_cast<uint32_t>(command.instanceModels.size());
    const size_t bytes = instanceCount * sizeof(glm::mat4);
    uint8_t* mem = new uint8_t[bytes];
    // Copy matrices
    for (uint32_t i = 0; i < instanceCount; ++i)
    {
        std::memcpy(&mem[i * sizeof(glm::mat4)], &command.instanceModels[i], sizeof(glm::mat4));
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

    command.vertexBuffer->AttachInstanceStream(std::static_pointer_cast<IBuffer>(instanceBuffer), attrs, BufferUsage::StaticDraw);
    renderSystem.DrawIndexedInstanced(command.indexBuffer, instanceCount);
    command.vertexBuffer->DetachInstanceStreams();
    // free temporary allocation (OwningBuffer::Take took ownership)
    // OwningBuffer destructor will free it
    return true;
}

void SceneRenderer::SetAmbientLightParams(Ref<GpuProgram> gpuProgram, const entt::registry &registry)
{
    rmt_ScopedCPUSample(SceneRenderer_SetAmbientLightParams, 0);
    const auto ambientLightEnt = registry.view<AmbientLightComponent>().front();
    const auto color = ambientLightEnt == entt::null ? Color3F::Grey : registry.get<AmbientLightComponent>(ambientLightEnt).color;
    gpuProgram->SetUniformVector("_AmbientLight", color);
}

void SceneRenderer::SetDirectLightParams(Ref<GpuProgram> gpuProgram, const entt::registry& registry)
{
    rmt_ScopedCPUSample(SceneRenderer_SetDirectLightParams, 0);
    int32_t numLight = 0;
    for (const auto view = registry.view<SceneNodeComponent, DirectLightComponent>(); const auto& [entity, node, light] : view.each())
    {
        if (!node.active) continue;

        rmt_BeginCPUSample(CreateUniformNames, 0);
        // TODO: Cache the uniform names.
        const auto uniformLocation = std::format("_DirectLights[{}]", numLight);
        const auto uniformLocationColor = std::format("{}.properties.color", uniformLocation);
        const auto uniformLocationDiffuse = std::format("{}.properties.diffuseIntensity", uniformLocation);
        const auto uniformLocationSpecular = std::format("{}.properties.specularIntensity", uniformLocation);
        const auto uniformLocationCutoff = std::format("{}.properties.cutoff", uniformLocation);
        const auto uniformLocationDirection = std::format("{}.direction", uniformLocation);
        rmt_EndCPUSample();

        rmt_BeginCPUSample(EmplaceUniformValues, 0);
        gpuProgram->SetUniformVector(uniformLocationColor, light.properties.color);
        gpuProgram->SetUniformVector(uniformLocationDirection, glm::normalize(light.direction));
        gpuProgram->SetUniformFloat(uniformLocationDiffuse, light.properties.diffuseIntensity);
        gpuProgram->SetUniformFloat(uniformLocationSpecular, light.properties.specularIntensity);
        gpuProgram->SetUniformFloat(uniformLocationCutoff, light.properties.cutoffRange);
        rmt_EndCPUSample();

        numLight++;
    }
    gpuProgram->SetUniformInt("_NumDirectLight", numLight);
}

void SceneRenderer::SetPointLightParams(Ref<GpuProgram> gpuProgram, const entt::registry& registry)
{
    rmt_ScopedCPUSample(SceneRenderer_SetPointLightParams, 0);
    int32_t numLight = 0;
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

void SceneRenderer::SetCameraUniforms(Ref<GpuProgram> gpuProgram, const glm::vec3& cameraPos, const glm::mat4& viewMtx, const glm::mat4& projectionMtx)
{
    rmt_ScopedCPUSample(SceneRenderer_SetCameraUniforms, 0);
    gpuProgram->SetUniformVector("_CameraPos", cameraPos);
    gpuProgram->SetUniformMatrix("_View", viewMtx, false);
    gpuProgram->SetUniformMatrix("_Projection", projectionMtx, false);
}

void SceneRenderer::Render(RenderSystem &renderSystem, const Scene &scene)
{
    rmt_ScopedCPUSample(SceneRenderer_Render, 0);

    std::vector<RenderCommand> commands;
    commands.reserve(1024);

    const auto& registry = *scene.GetRegistry();
    auto renderInterface = renderSystem.GetRenderInterface();

    for (auto pass : m_renderPasses)
    {
        if (!pass->enabled) continue;

        commands.clear();

        // Begin per-pass CPU and GPU (OpenGL) samples using dynamic names
        rmt_BeginCPUSampleDynamic(pass->name.c_str(), 0);
        rmt_BeginOpenGLSampleDynamic(pass->name.c_str());

        std::vector<Ref<SceneView>> viewPoints;
        pass->PopulateSceneView(scene, viewPoints);

        for (const auto view : viewPoints)
        {
            if (!view) continue;

            rmt_BeginCPUSample(SceneRenderer_CreateSortList, 0)
            const glm::vec3& cameraPos = view->GetViewPos();
            const glm::mat4& viewMtx = view->GetViewMatrix();
            const glm::mat4& projectionMtx = view->GetProjectionMatrix();
            const glm::mat4& viewProjMtx = view->GetViewProjectionMatrix();
            for (const auto entities = registry.view<SceneNodeComponent, MeshComponent, PositionComponent, OrientationComponent, ScaleComponent>();
                 const auto& [entity, sceneNode, meshComp, position, orient, scale] : entities.each())
            {
                if (!sceneNode.active) continue;

                glm::mat4 modelMtx = Matrix::CreateModelMatrix(position.value, orient.quat, scale.value);
                const auto mvpMtx = viewProjMtx * modelMtx;

                if (auto mesh = meshComp.GetMesh(); mesh != nullptr)
                {
                    if (!pass->IsPassFiltered(mesh)) continue;
                    if (!view->IsObjectVisible(mesh->GetSphere(), modelMtx, scale.value)) continue;

                    commands.push_back(pass->CreateRenderCommand(mesh, modelMtx, mvpMtx));
                }
            }
            rmt_EndCPUSample();

            {
                rmt_ScopedCPUSample(SceneRenderer_SortMeshes, 0)
                std::ranges::sort(commands, [](const RenderCommand& a, const RenderCommand& b) {
                    return a.sortKey < b.sortKey;
                });
            }
            
            pass->Begin(renderSystem);
            Ref<GpuProgram> globalShader = pass->pipelineState.globalShader;
            {
                rmt_ScopedCPUSample(SceneRenderer_RenderMeshes, 0)

                Ref<GpuProgram> usingProgram = nullptr;
                Ref<Material> usingMaterial = nullptr;
                for (const auto& cmd : commands)
                {
                    if (!cmd.material) continue;
                    auto material = cmd.material;

                    Ref<GpuProgram> gpuProgram = nullptr;
                    if (globalShader) {
                        gpuProgram = globalShader;
                    }
                    else {
                        gpuProgram = usingMaterial != material ? material->Use() : material->GetGpuProgram();
                        usingMaterial = material;
                    }
                    NXS_ASSERT_MSG(gpuProgram, "No shader program available for rendering");

                    if (gpuProgram != usingProgram)
                    {
                        if (!gpuProgram->IsBinding()) gpuProgram->Bind();

                        SetCameraUniforms(gpuProgram, cameraPos, viewMtx, projectionMtx);
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
            }
            pass->End(renderSystem);

            // End per-pass GPU and CPU samples
            rmt_EndOpenGLSample();
            rmt_EndCPUSample();
        }
    }
}

bool SceneRenderer::IsSphereInside(const Frustum &viewFustrum, const Sphere &sphere, glm::mat4 modelMtx, const glm::vec3 &scale)
{
    const glm::vec3 pos = modelMtx * glm::vec4(sphere.center, 1);
    const float maxScale = std::max(std::max(scale.x, scale.y), scale.z);
    const float scaledRadius = sphere.radius * maxScale;
    return viewFustrum.IsSphereInside(pos, scaledRadius);
}
