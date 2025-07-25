//
// Created by nutta on 7/25/2025.
//
#include "nexus/scene/SceneRenderer.h"

#include "graphics/RenderSystem.h"
#include "math/Matrix.h"
#include "nexus/ecs/graphics/RenderComponent.h"
#include "nexus/ecs/scene/CameraComponent.h"
#include "nexus/ecs/scene/TransformComponent.h"
#include "nexus/ecs/scene/LightComponent.h"
#include "nexus/graphics/RenderCommand.h"
#include "nexus/math/Math.h"
#include "nexus/ecs/Ecs.h"

USING_NAMESPACE_NXS;

void BasicSceneRenderer::Render(RenderSystem& renderSystem, const entt::registry& registry)
{
    // Lights
    const auto ambientLightEnt = registry.view<AmbientLightComponent>().front();
    const auto ambient = ambientLightEnt == entt::null ? COLOR3F_WHITE : registry.get<AmbientLightComponent>(ambientLightEnt).color;
    const auto directionalLights = ECS::FindAllComponents<DirectLightComponent>(registry);
    const auto pointLights = ECS::FindAllComponents<PointLightComponent>(registry);
    // There should be only one directional light in the scene.
    const auto directLight = directionalLights.size() > 0 ? directionalLights[0] : nullptr;

    // ReSharper disable once CppTooWideScopeInitStatement
    const auto cameraView = registry.view<CameraComponent, PositionComponent, RotationComponent>();
    for (auto [cameraEntity, camera, cameraPos, cameraOrient] : cameraView.each())
    {
        glm::mat4 viewMtx = Matrix::CreateViewMatrix(cameraPos.value, cameraOrient.value);
        glm::mat4 projection;
        if (camera.projectionType == ProjectionType::Perspective) {
            projection = glm::perspective(glm::radians(camera.fov), camera.width / camera.height, camera.nearZ, camera.farZ);
        }
        else {
            projection = glm::ortho(-camera.width/2, camera.width/2, -camera.height/2, camera.height/2, camera.nearZ, camera.farZ);
        }

        for (const auto view = registry.view<RenderComponent, TransformComponent>(); auto [entity, renderComponent, transform] : view.each())
        {
            glm::mat4 modelMtx = Matrix::CreateModelMatrix(transform.translation, transform.rotation, transform.scale);
            RenderCommand renderCommand = {
                renderComponent.shader,
                renderComponent.vertexBuffer,
                renderComponent.indexBuffer,
                {
                    {"model", modelMtx},
                    {"view", viewMtx},
                    {"projection", projection},
                },
                // Textures
                {},
                {
                    // TODO: Multiple light sources.
                    { "u_Ambient", ambient },
                    { "u_Light.position", directLight ? directLight->direction : glm::vec3(0.0f) },
                    { "u_Light.diffuse", directLight ? directLight->light.diffuseColor : glm::vec3(0.0f) },
                }
            };

            if (auto textureComponent = registry.try_get<DiffuseMapComponent>(entity))
            {
                auto& textures = textureComponent->textures;
                for (int i = 0; i < textures.size(); i++)
                {
                    auto texture = textures[i];
                    renderCommand.uniform2DTextures.emplace_back("ourTexture", i, texture);
                }
            }

            renderSystem.RegisterDrawCommand(renderCommand);
        }
    }
}
