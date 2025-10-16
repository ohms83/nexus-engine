//
// Created by nutta on 7/25/2025.
//
#include "scene/SceneRenderer.h"

#include <format>

#include "core/Logger.h"
#include "graphics/debug/Gizmos.h"
#include "graphics/RenderSystem.h"
#include "graphics/RenderCommand.h"
#include "geom/Frustum.h"
#include "ecs/component/graphics/RenderComponent.h"
#include "ecs/component/graphics/ModelComponent.h"
#include "ecs/Ecs.h"
#include "math/Math.h"
#include "math/Matrix.h"
#include "scene/component/CameraProperties.h"
#include "scene/component/LightComponent.h"
#include "scene/component/SceneNodeComponent.h"
#include "scene/component/TransformComponent.h"

#include "Remotery.h"

USING_NAMESPACE_NXS;

DEFINE_LOG(BasicSceneRenderer);

static void SetAmbientLightParams(RenderCommand& command, const entt::registry& registry)
{
    rmt_ScopedCPUSample(SceneRenderer_SetAmbientLightParams, 0);
    const auto ambientLightEnt = registry.view<AmbientLightComponent>().front();
    const auto color = ambientLightEnt == entt::null ? Color3F::Grey : registry.get<AmbientLightComponent>(ambientLightEnt).color;
    command.uniformVec3.emplace_back("_AmbientLight", color);
}

static void SetDirectLightParams(RenderCommand& command, const entt::registry& registry)
{
    rmt_ScopedCPUSample(SceneRenderer_SetDirectLightParams, 0);
    const auto lights = ECS::FindAllComponents<DirectLightComponent>(registry);
    const auto numLights = CAST<int32>(lights.size());

    for (int i = 0; i < lights.size(); ++i)
    {
        const auto light = lights[i];
        if (!light) continue;

        const auto uniformLocation = std::format("_DirectLights[{}]", i);
        const auto uniformLocationColor = std::format("{}.properties.color", uniformLocation);
        const auto uniformLocationDiffuse = std::format("{}.properties.diffuseIntensity", uniformLocation);
        const auto uniformLocationSpecular = std::format("{}.properties.specularIntensity", uniformLocation);
        const auto uniformLocationCutoff = std::format("{}.properties.cutoff", uniformLocation);
        const auto uniformLocationDirection = std::format("{}.direction", uniformLocation);

        command.uniformVec3.emplace_back(uniformLocationColor, light->properties.color);
        command.uniformVec3.emplace_back(uniformLocationDirection, light->direction);
        command.uniformFloats.emplace_back(uniformLocationDiffuse, light->properties.diffuseIntensity);
        command.uniformFloats.emplace_back(uniformLocationSpecular, light->properties.specularIntensity);
        command.uniformFloats.emplace_back(uniformLocationCutoff, light->properties.cutoffRange);
    }
    command.uniformInts.emplace_back("_NumDirectLight", INT_CAST(lights.size()));
}

static void SetPointLightParams(RenderCommand& command, const entt::registry& registry)
{
    rmt_ScopedCPUSample(SceneRenderer_SetPointLightParams, 0);
    int32 numLight = 0;
    for (const auto view = registry.view<PointLightComponent, PositionComponent>(); const auto& [entity, light, position] : view.each())
    {
        rmt_BeginCPUSample(CreateUniformNames, 0);
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
        command.uniformVec3.emplace_back(uniformLocationPosition, position.value);
        command.uniformVec3.emplace_back(uniformLocationColor, light.properties.color);
        command.uniformFloats.emplace_back(uniformLocationDiffuse, light.properties.diffuseIntensity);
        command.uniformFloats.emplace_back(uniformLocationSpecular, light.properties.specularIntensity);
        command.uniformFloats.emplace_back(uniformLocationCutOff, light.properties.cutoffRange);
        command.uniformFloats.emplace_back(uniformLocationConst, light.constant);
        command.uniformFloats.emplace_back(uniformLocationLinear, light.linear);
        command.uniformFloats.emplace_back(uniformLocationQuad, light.quadratic);
        rmt_EndCPUSample();

        numLight++;
    }
    command.uniformInts.emplace_back("_NumPointLight", numLight);
}

static void SetTextureParams(RenderCommand& command, const entt::registry& registry, const entt::entity& entity)
{
    rmt_ScopedCPUSample(SceneRenderer_SetTextureParams, 0);
    if (const auto textureComponent = registry.try_get<DiffuseMapComponent>(entity))
    {
        auto& textures = textureComponent->textures;
        for (int i = 0; i < textures.size(); i++)
        {
            auto texture = textures[i];
            command.uniform2DTextures.emplace_back("_DiffuseMap", i, texture);
        }
    }
}

void BasicSceneRenderer::Render(RenderSystem& renderSystem, const entt::registry& registry)
{
    // ReSharper disable once CppTooWideScopeInitStatement
    const auto cameraView = registry.view<SceneNodeComponent, CameraProperties, PositionComponent, OrientationComponent>();
    for (const auto& [cameraEntity, sceneNode, camera, cameraPos, cameraOrient] : cameraView.each())
    {
        if (!sceneNode.active) continue;

        glm::mat4 viewMtx = Matrix::CreateViewMatrix(cameraPos.value, cameraOrient.value);
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
            if (!model.model) continue;

            glm::mat4 modelMtx = Matrix::CreateModelMatrix(position.value, orient.value, scale.value);
            const auto& sphere = model.model->GetBoundingSphere();
            const glm::vec3 transformedCenter = modelMtx * glm::vec4(sphere.center, 1);
            // TODO: Handle non-uniform scaling.
            const float scaledRadius = sphere.radius * scale.value.x;

            if (!viewFrustum.IsSphereInside(transformedCenter, scaledRadius)) continue;

            rmt_BeginCPUSample(SceneRenderer_CreateDrawCommand, 0)
            {
                auto commands = model.model->CreateDrawCommand();
                for (auto& command : commands)
                {
                    rmt_ScopedCPUSample(SceneRenderer_SetCommandParams, 0);
                    command.uniformMatrices.emplace("_Model", modelMtx);
                    command.uniformMatrices.emplace("_View", viewMtx);
                    command.uniformMatrices.emplace("_Projection", projection);

                    SetAmbientLightParams(command, registry);
                    SetDirectLightParams(command, registry);
                    SetPointLightParams(command, registry);
                    SetTextureParams(command, registry, entity);

                    renderSystem.RegisterDrawCommand(command);
                }
            }
            rmt_EndCPUSample();
        }

        Gizmos::ProcessDraw(renderSystem, projection * viewMtx);
        // Only render from the first active camera POV.
        break;
    }
}
