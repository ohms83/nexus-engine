//
// Created by nutta on 7/25/2025.
//
#include "scene/renderer/SceneRenderer.h"

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
    const auto instanceCount = static_cast<uint32>(command.instanceModels.size());
    const size_t bytes = instanceCount * sizeof(glm::mat4);
    uint8_t* mem = new uint8_t[bytes];
    // Copy matrices
    for (uint32 i = 0; i < instanceCount; ++i)
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
        gpuProgram->SetUniformVector(uniformLocationDirection, glm::normalize(light.direction));
        gpuProgram->SetUniformFloat(uniformLocationDiffuse, light.properties.diffuseIntensity);
        gpuProgram->SetUniformFloat(uniformLocationSpecular, light.properties.specularIntensity);
        gpuProgram->SetUniformFloat(uniformLocationCutoff, light.properties.cutoffRange);

        numLight++;
    }
    gpuProgram->SetUniformInt("_NumDirectLight", numLight);
}

void SceneRenderer::SetPointLightParams(Ref<GpuProgram> gpuProgram, const entt::registry& registry)
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

void SceneRenderer::SetCameraUniforms(Ref<GpuProgram> gpuProgram, const glm::vec3& cameraPos, const glm::mat4& viewMtx, const glm::mat4& projectionMtx)
{
    gpuProgram->SetUniformVector("_CameraPos", cameraPos);
    gpuProgram->SetUniformMatrix("_View", viewMtx, false);
    gpuProgram->SetUniformMatrix("_Projection", projectionMtx, false);
}

bool SceneRenderer::IsSphereInside(const Frustum& viewFustrum, const Sphere& sphere, glm::mat4 modelMtx, const glm::vec3& scale)
{
    const glm::vec3 pos = modelMtx * glm::vec4(sphere.center, 1);
    const float maxScale = std::max(std::max(scale.x, scale.y), scale.z);
    const float scaledRadius = sphere.radius * maxScale;
    return viewFustrum.IsSphereInside(pos, scaledRadius);
}
