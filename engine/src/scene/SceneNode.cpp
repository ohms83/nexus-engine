#include <utility>

#include "nexus/scene/SceneNode.h"

#include <format>

#include "ecs/component/scene/SceneNodeComponent.h"
#include "ecs/component/scene/TransformComponent.h"
#include "scene/Scene.h"

USING_NAMESPACE_NXS;

SceneNode::SceneNode(entt::registry& registry)
    : Entity(registry)
    , m_node(AddComponent<SceneNodeComponent>())
{
}

SceneNode::SceneNode(entt::registry& registry, std::string name)
    : Entity(registry)
    , m_node(AddComponent<SceneNodeComponent>())
{
    m_node.name = name;
}

void SceneNode::Activate(const bool activate)
{
    m_node.active = activate;
    if (activate) OnActivate();
    else OnDeactivate();
}

void SceneNode::Translate(const glm::vec3& translation)
{
    if (const auto component = TryGetComponent<PositionComponent>()) {
        component->value += translation;
    }
    // TODO: Warning
}

void SceneNode::Rotate(const float degree, const glm::vec3& axis)
{
    // Try TransformComponent first
    if (const auto component = TryGetComponent<TransformComponent>()) {
        component->rotation = glm::rotate(component->rotation, glm::radians(degree), axis);
    }
    if (const auto component = TryGetComponent<OrientationComponent>()) {
        component->value = glm::rotate(component->value, glm::radians(degree), axis);
    }
    // TODO: Warning
}

void SceneNode::Rotate(const glm::vec3& eulerAngles)
{
    // Try TransformComponent first
    const auto eulerAngleRadians = glm::radians(eulerAngles);
    if (const auto component = TryGetComponent<TransformComponent>()) {
        component->rotation *= glm::quat(eulerAngleRadians);
    }
    if (const auto component = TryGetComponent<OrientationComponent>()) {
        component->value *= glm::quat(eulerAngleRadians);
    }
}

void SceneNode::Scale(const glm::vec3& scale)
{
    if (const auto& component = TryGetComponent<ScaleComponent>()) {
        component->value *= scale;
    }
    // TODO: Warning
}

void SceneNode::SetPosition(const glm::vec3& position)
{
    if (const auto component = TryGetComponent<PositionComponent>()) {
        component->value = position;
    }
    // TODO: Warning
}

const glm::vec3& SceneNode::GetPosition() const
{
    if (const auto component = TryGetComponent<PositionComponent>()) {
        return component->value;
    }
    // TODO: Warning
    static constexpr glm::vec3 zero(0, 0, 0);
    return zero;
}

void SceneNode::SetRotation(const glm::quat& rotation)
{
    if (const auto component = TryGetComponent<OrientationComponent>()) {
        component->value = rotation;
    }
    // TODO: Warning
}

const glm::quat& SceneNode::GetRotation() const
{
    if (const auto component = TryGetComponent<OrientationComponent>()) {
        return component->value;
    }
    static constexpr glm::quat identity(1, 0, 0, 0);
    return identity;
}

void SceneNode::SetScale(const glm::vec3& scale)
{
    if (const auto component = TryGetComponent<ScaleComponent>()) {
        component->value = scale;
    }
    // TODO: Warning
}

const glm::vec3& SceneNode::GetScale() const
{
    if (const auto& component = TryGetComponent<ScaleComponent>()) {
        return component->value;
    }
    static constexpr glm::vec3 one(1, 1, 1);
    return one;
}

void SceneNode::LookAt(const glm::vec3& center, const glm::vec3& up)
{
    SetRotation(glm::quatLookAt(glm::normalize(center - GetPosition()), up));
}
