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
#include "geom/Frustum.h"
#include "ecs/Ecs.h"
#include "math/Math.h"
#include "math/Matrix.h"
#include "scene/component/CameraProperties.h"
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

DEFINE_LOG(BasicSceneRenderer);

namespace
{
    //! @brief A 64 bit sort-key.
    struct SortKey
    {
        //! @brief Translucency flag. 0 means opague. 1 means translucent.
        uint8_t translucent : 1;
        //! @brief Material's ID
        uint32_t materialId : 31;
        //! @brief Mesh's Z value in the viewport space (0..1) normalized to an integer.
        uint32_t normalizedZ : 32;

        void SetZValue(float z)
        {
            normalizedZ = CAST<uint32_t>(Math::Lerp<float>(0, UINT32_MAX, z));
        }

        bool operator < (const SortKey& rhs)
        {
            return *(uint64_t*)this < *(uint64_t*)&rhs;
        }
    };

    struct SortedMesh
    {
        uint64_t key;
        Mesh* mesh;
        const glm::mat4* modelMatrix;
    };

    std::vector<SortedMesh> s_sortedMeshes;
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
    // TODO: Handle non-uniform scaling.
    const float scaledRadius = sphere.radius * scale.x;
    return viewFustrum.IsSphereInside(pos, scaledRadius);
}

BasicSceneRenderer::BasicSceneRenderer(const RenderSystem& renderSystem)
{
    Hasher hasher;
    const auto name = "_DepthShader";
    m_depthShader = std::make_shared<Shader>(name, hasher.Hash32(name));
    m_depthShader->CompileFromSource(*renderSystem.GetRenderInterface(), s_depthVertexShader, s_depthFragmentShader);
}

void BasicSceneRenderer::Render(RenderSystem& renderSystem, const entt::registry& registry)
{
    m_commandBuffer.clear();
    s_sortedMeshes.clear();

    // Storing mesh's model matrix for the rendering phase.
    std::vector<glm::mat4> modelMatrices;

    // ReSharper disable once CppTooWideScopeInitStatement
    const auto cameraView = registry.view<SceneNodeComponent, CameraProperties, PositionComponent, OrientationComponent>();
    for (const auto& [cameraEntity, sceneNode, camera, cameraPos, cameraOrient] : cameraView.each())
    {
        if (!sceneNode.active) continue;

        glm::mat4 viewMtx = Matrix::CreateViewMatrix(cameraPos.value, cameraOrient.quat);
        glm::mat4 projection;
        if (camera.projectionType == ProjectionType::Perspective) {
            projection = glm::perspective(glm::radians(camera.fov), camera.width / camera.height, camera.nearZ, camera.farZ);
        }
        else {
            projection = glm::ortho(-camera.width/2, camera.width/2, -camera.height/2, camera.height/2, camera.nearZ, camera.farZ);
        }

        const auto viewProjMtx = projection * viewMtx;
        const auto viewFrustum = Frustum::CreateViewFrustum(viewProjMtx);
        for (const auto view = registry.view<SceneNodeComponent, ModelComponent, PositionComponent, OrientationComponent, ScaleComponent>(); const auto& [entity, sceneNode, model, position, orient, scale] : view.each())
        {
            if (!sceneNode.active || !model.model) continue;

            const glm::mat4& modelMtx = modelMatrices.emplace_back(Matrix::CreateModelMatrix(position.value, orient.quat, scale.value));
            const auto& sphere = model.model->GetBoundingSphere();
            // TODO: Handle non-uniform scaling.
            const float scaledRadius = sphere.radius * scale.value.x;

            if (!IsSphereInside(viewFrustum, sphere, modelMtx, scale.value)) continue;

            rmt_BeginCPUSample(SceneRenderer_CreateSortList, 0)
            const auto mvpMtx = projection * viewMtx * modelMtx;
            for (auto mesh : model.model->GetMeshes())
            {
                const auto material = mesh->GetMaterial();
                const auto meshSphere = mesh->GetSphere();
                const auto pos = mvpMtx * glm::vec4(meshSphere.center, 1);
                const auto clipZ = pos.z / pos.w;
                uint64_t translucent = 0;
                uint64_t materialId = 0x7FFFFFFF & material->GetId();
                uint64_t depth = CAST<uint32_t>(Math::Lerp<float>(0, UINT32_MAX, clipZ));
                uint64_t key = translucent << 63 | materialId << 32 | depth;
                s_sortedMeshes.emplace_back(key, mesh.get(), &modelMtx);
            }
            rmt_EndCPUSample();
        }

        {
            rmt_ScopedCPUSample(SceneRenderer_SortMeshes, 0)
            std::sort(s_sortedMeshes.begin(), s_sortedMeshes.end(), [](const SortedMesh& a, const SortedMesh& b) {
                return a.key < b.key;
            });
        }
        {
            // LOG_DEBUG(LogBasicSceneRenderer, std::format("Begin Draw..."));
            rmt_ScopedCPUSample(SceneRenderer_RenderMeshes, 0)
            Material* usingMaterial = nullptr;
            for (const auto [key, mesh, modelMtx] : s_sortedMeshes)
            {
                auto material = mesh->GetMaterial();

                if (material.get() != usingMaterial)
                {
                    // LOG_DEBUG(LogBasicSceneRenderer, std::format("Switch material={}", material->GetPath()));
                    material->Use();
                    usingMaterial = material.get();

                    auto gpuProgram = material->GetShader()->GetGpuProgram();
                    gpuProgram->SetUniformMatrix("_Model", *modelMtx, false);
                    gpuProgram->SetUniformMatrix("_View", viewMtx, false);
                    gpuProgram->SetUniformMatrix("_Projection", projection, false);

                    SetAmbientLightParams(gpuProgram, registry);
                    SetDirectLightParams(gpuProgram, registry);
                    SetPointLightParams(gpuProgram, registry);
                }

                mesh->GetVertexBuffer()->Bind();
                renderSystem.DrawIndexed(mesh->GetIndexBuffer());
            }
            // LOG_DEBUG(LogBasicSceneRenderer, std::format("End Draw"));
        }

        Gizmos::ProcessDraw(renderSystem, projection * viewMtx);
        // Only render from the first active camera POV.
        break;
    }
}
