//
// Created by nutta on 7/26/2025.
//
#include "scene/system/SceneNodeTransformSystem.h"
#include "scene/component/SceneNodeComponent.h"
#include "scene/component/TransformComponent.h"

namespace nxs
{
    void MoveNode(entt::registry& registry, float dt)
    {
        const auto view = registry.view<PositionComponent, const MoveComponent>();
        view.each([dt](PositionComponent& postion, const MoveComponent& move)
        {
            postion.value += move.direction * move.speed * dt;
        });
    }

    void RotateNode(entt::registry& registry, float dt)
    {
        const auto view = registry.view<TransformComponent, const RotationComponent>();
        view.each([dt](TransformComponent& transform, const RotationComponent& rotation)
        {
            transform.rotation = glm::rotate(transform.rotation, glm::radians(rotation.degree * dt), rotation.axis);
        });
    }
}
