//
// Created by nutta on 7/25/2025.
//
#include "nexus/scene/SceneRenderer.h"

#include <format>

#include "core/Logger.h"
#include "graphics/RenderSystem.h"
#include "math/Matrix.h"
#include "nexus/ecs/component/graphics/RenderComponent.h"
#include "nexus/ecs/component/graphics/ModelComponent.h"
#include "nexus/ecs/component/scene/CameraComponent.h"
#include "nexus/ecs/component/scene/TransformComponent.h"
#include "nexus/ecs/component/scene/LightComponent.h"
#include "nexus/graphics/RenderCommand.h"
#include "nexus/math/Math.h"
#include "nexus/ecs/Ecs.h"

USING_NAMESPACE_NXS;

DEFINE_LOG(BasicSceneRenderer);

static void SetAmbientLightParams(RenderCommand& command, const entt::registry& registry)
{
    const auto ambientLightEnt = registry.view<AmbientLightComponent>().front();
    const auto color = ambientLightEnt == entt::null ? Color3F::Grey : registry.get<AmbientLightComponent>(ambientLightEnt).color;
    command.uniformVec3.emplace_back("_Ambient", color);
}

static void SetDirectLightParams(RenderCommand& command, const entt::registry& registry)
{
    const auto lights = ECS::FindAllComponents<DirectLightComponent>(registry);
    const auto numLights = CAST<int32>(lights.size());

    if (lights.empty()) return;

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
    const auto lights = ECS::FindAllComponents<PointLightComponent>(registry);
    const auto numLights = CAST<int32>(lights.size());

    if (lights.empty()) return;

    for (int i = 0; i < lights.size(); ++i)
    {
        const auto light = lights[i];
        if (!light) continue;

        const auto uniformLocation = std::format("_PointLights[{}]", i);
        const auto uniformLocationColor = std::format("{}.properties.color", uniformLocation);
        const auto uniformLocationDiffuse = std::format("{}.properties.diffuseIntensity", uniformLocation);
        const auto uniformLocationSpecular = std::format("{}.properties.specularIntensity", uniformLocation);
        const auto uniformLocationCutOff = std::format("{}.properties.cutoff", uniformLocation);
        const auto uniformLocationPosition = std::format("{}.position", uniformLocation);
        const auto uniformLocationConst = std::format("{}.constant", uniformLocation);
        const auto uniformLocationLinear = std::format("{}.linear", uniformLocation);
        const auto uniformLocationQuad = std::format("{}.quadratic", uniformLocation);

        command.uniformVec3.emplace_back(uniformLocationPosition, light->position);
        command.uniformVec3.emplace_back(uniformLocationColor, light->properties.color);
        command.uniformFloats.emplace_back(uniformLocationDiffuse, light->properties.diffuseIntensity);
        command.uniformFloats.emplace_back(uniformLocationSpecular, light->properties.specularIntensity);
        command.uniformFloats.emplace_back(uniformLocationCutOff, light->properties.cutoffRange);
        command.uniformFloats.emplace_back(uniformLocationConst, light->constant);
        command.uniformFloats.emplace_back(uniformLocationLinear, light->linear);
        command.uniformFloats.emplace_back(uniformLocationQuad, light->quadratic);
    }

    command.uniformInts.emplace_back("_NumPointLight", INT_CAST(lights.size()));
}

static void SetTextureParams(RenderCommand& command, const entt::registry& registry, const entt::entity& entity)
{
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
    const auto cameraView = registry.view<CameraComponent, PositionComponent, OrientationComponent>();
    for (const auto& [cameraEntity, camera, cameraPos, cameraOrient] : cameraView.each())
    {
        glm::mat4 viewMtx = Matrix::CreateViewMatrix(cameraPos.value, cameraOrient.value);
        glm::mat4 projection;
        if (camera.projectionType == ProjectionType::Perspective) {
            projection = glm::perspective(glm::radians(camera.fov), camera.width / camera.height, camera.nearZ, camera.farZ);
        }
        else {
            projection = glm::ortho(-camera.width/2, camera.width/2, -camera.height/2, camera.height/2, camera.nearZ, camera.farZ);
        }

        for (const auto view = registry.view<ModelComponent, TransformComponent>(); const auto& [entity, modelComponent, transform] : view.each())
        {
            glm::mat4 modelMtx = Matrix::CreateModelMatrix(transform.translation, transform.rotation, transform.scale);
            auto commands = modelComponent.model->CreateDrawCommand();
            for (auto& command : commands)
            {
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
    }
}
