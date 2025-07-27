//
// Created by nutta on 7/26/2025.
//
#include "nexus/ecs/system/scene/SceneNodeTransformSystem.h"

#include "ecs/component/scene/SceneNodeComponent.h"
#include "nexus/ecs/component/scene/TransformComponent.h"

namespace nxs
{
    void MoveNode(entt::registry& registry, float dt)
    {
        const auto view = registry.view<const SceneNodeComponent, const MoveComponent>();
        view.each([dt](const SceneNodeComponent& node, const MoveComponent& move)
        {
            node.ref.Translate(move.direction * move.speed * dt);
        });
    }

    void RotateNode(entt::registry& registry, float dt)
    {
        const auto view = registry.view<const SceneNodeComponent, const RotationComponent>();
        view.each([dt](const SceneNodeComponent& node, const RotationComponent& rotation)
        {
            node.ref.Rotate(rotation.degree * dt, rotation.axis);
        });
    }
}
