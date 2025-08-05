//
// Created by nutta on 7/25/2025.
//
#include "nexus/scene/SceneRenderer.h"

#include <format>

#include "core/Logger.h"
#include "graphics/RenderSystem.h"
#include "math/Matrix.h"
#include "nexus/ecs/component/graphics/RenderComponent.h"
#include "nexus/ecs/component/scene/CameraComponent.h"
#include "nexus/ecs/component/scene/TransformComponent.h"
#include "nexus/ecs/component/scene/LightComponent.h"
#include "nexus/graphics/RenderCommand.h"
#include "nexus/math/Math.h"
#include "nexus/ecs/Ecs.h"

USING_NAMESPACE_NXS;

DEFINE_LOG(BasicSceneRenderer);

static void SetLightParams(RenderCommand& command, const entt::registry& registry)
{
    // Lights
    const auto ambientLightEnt = registry.view<AmbientLightComponent>().front();
    const auto ambient = ambientLightEnt == entt::null ? Color3F::Grey : registry.get<AmbientLightComponent>(ambientLightEnt).color;
    const auto directionalLights = ECS::FindAllComponents<DirectLightComponent>(registry);
    const auto pointLights = ECS::FindAllComponents<PointLightComponent>(registry);
    // There should be only one directional light in the scene.
    const auto directLight = !directionalLights.empty() ? directionalLights[0] : nullptr;

    command.uniformVec3.emplace_back("u_Ambient", ambient);
    command.uniformVec3.emplace_back("u_DirectLight.direction", directLight ? directLight->direction : glm::vec3(0.0f));
    command.uniformVec3.emplace_back("u_DirectLight.diffuse", directLight ? directLight->light.diffuseColor : glm::vec3(0.0f));

    // Point lights
    const auto numPointLights = CAST<int32>(pointLights.size());
    command.uniformInts.emplace_back("u_NumPointLight", numPointLights);

    for (size_t i = 0; i < numPointLights; ++i)
    {
        const auto uniformLocation = std::format("u_PointLights[{}]", i);
        const auto unitformDiffuseColor = std::format("{}.diffuse", uniformLocation);
        const auto unitformSpecularColor = std::format("{}.specular", uniformLocation);
        const auto unitformEmissiveColor = std::format("{}.emissive", uniformLocation);
        const auto unitformCutoff = std::format("{}.cutoff", uniformLocation);
        const auto unitformPosition = std::format("{}.position", uniformLocation);
        const auto unitformConstantAtten = std::format("{}.costant", uniformLocation);
        const auto unitformLinearAtten = std::format("{}.linear", uniformLocation);
        const auto unitformQuadraticAtten = std::format("{}.quadratic", uniformLocation);
        
        command.uniformVec3.emplace_back(unitformDiffuseColor, pointLights[i]->light.diffuseColor);
        command.uniformVec3.emplace_back(unitformSpecularColor, pointLights[i]->light.specularColor);
        command.uniformVec3.emplace_back(unitformEmissiveColor, pointLights[i]->light.emissiveColor);
        command.uniformVec3.emplace_back(unitformPosition, pointLights[i]->position);

        command.uniformFloats.emplace_back(unitformCutoff, pointLights[i]->light.cutoffRange);
        command.uniformFloats.emplace_back(unitformConstantAtten, pointLights[i]->constantAttenuation);
        command.uniformFloats.emplace_back(unitformLinearAtten, pointLights[i]->linearAttenuation);
        command.uniformFloats.emplace_back(unitformQuadraticAtten, pointLights[i]->quadraticAttenuation);
    }
}

static void SetTextureParams(RenderCommand& command, const entt::registry& registry, const entt::entity& entity)
{
    if (const auto textureComponent = registry.try_get<DiffuseMapComponent>(entity))
    {
        auto& textures = textureComponent->textures;
        for (int i = 0; i < textures.size(); i++)
        {
            auto texture = textures[i];
            command.uniform2DTextures.emplace_back("u_DiffuseMap", i, texture);
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

        for (const auto view = registry.view<RenderComponent, TransformComponent>(); const auto& [entity, renderComponent, transform] : view.each())
        {
            glm::mat4 modelMtx = Matrix::CreateModelMatrix(transform.translation, transform.rotation, transform.scale);
            RenderCommand command = {
                renderComponent.shader,
                renderComponent.vertexBuffer,
                renderComponent.indexBuffer,
                {
                    {"u_Model", modelMtx},
                    {"u_View", viewMtx},
                    {"u_Projection", projection},
                },
            };

            SetLightParams(command, registry);
            SetTextureParams(command, registry, entity);

            renderSystem.RegisterDrawCommand(command);
        }
    }
}
