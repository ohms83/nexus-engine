//
// Created by nutta on 7/26/2025.
//
#include "nexus/scene/system/SceneNodeTransformSystem.h"
#include "nexus/scene/component/SceneNodeComponent.h"
#include "nexus/scene/component/TransformComponent.h"

namespace nxs
{
    void MoveNode(entt::registry& registry, float dt)
    {
        const auto view = registry.view<TransformComponent, const MoveComponent>();
        view.each([dt](TransformComponent& transform, const MoveComponent& move)
        {
            transform.Translate(move.direction * move.speed * dt);
        });
    }

    void RotateNode(entt::registry& registry, float dt)
    {
        const auto view = registry.view<TransformComponent, const TurningComponent>();
        view.each([dt](TransformComponent& transform, const TurningComponent& rotation)
        {
            transform.Rotate(rotation.degree * dt, rotation.axis);
        });
    }
}
