#include <utility>

#include "nexus/scene/SceneNode.h"

#include <format>

#include "ecs/component/scene/SceneNodeComponent.h"
#include "ecs/component/scene/TransformComponent.h"
#include "scene/Scene.h"

USING_NAMESPACE_NXS;

SceneNode::SceneNode(entt::registry& registry)
    : m_registry(&registry)
{
    m_entity = registry.create();
    registry.emplace<SceneNodeComponent>(m_entity, *this);
}

SceneNode::SceneNode(entt::registry& registry, std::string  name)
    : m_name(std::move(name))
    , m_registry(&registry)
{
    m_entity = registry.create();
    registry.emplace<SceneNodeComponent>(m_entity, *this);
}

void SceneNode::Activate(const bool activate)
{
    m_active = activate;
    if (activate) OnActivate();
    else OnDeactivate();
}

void SceneNode::Translate(const glm::vec3& translation)
{
    if (const auto component = m_registry->try_get<PositionComponent>(m_entity)) {
        component->value += translation;
    }
    // TODO: Warning
}

void SceneNode::Rotate(const float degree, const glm::vec3& axis)
{
    // Try TransformComponent first
    if (const auto component = m_registry->try_get<TransformComponent>(m_entity)) {
        component->rotation = glm::rotate(component->rotation, glm::radians(degree), axis);
    }
    if (const auto component = m_registry->try_get<OrientationComponent>(m_entity)) {
        component->value = glm::rotate(component->value, glm::radians(degree), axis);
    }
    // TODO: Warning
}

void SceneNode::Rotate(const glm::vec3& eulerAngles)
{
    // Try TransformComponent first
    const auto eulerAngleRadians = glm::radians(eulerAngles);
    if (const auto component = m_registry->try_get<TransformComponent>(m_entity)) {
        component->rotation *= glm::quat(eulerAngleRadians);
    }
    if (const auto component = m_registry->try_get<OrientationComponent>(m_entity)) {
        component->value *= glm::quat(eulerAngleRadians);
    }
}

void SceneNode::Scale(const glm::vec3& scale)
{
    if (const auto& component = m_registry->try_get<ScaleComponent>(m_entity)) {
        component->value *= scale;
    }
    // TODO: Warning
}

void SceneNode::SetPosition(const glm::vec3& position)
{
    if (const auto component = m_registry->try_get<PositionComponent>(m_entity)) {
        component->value = position;
    }
    // TODO: Warning
}

const glm::vec3& SceneNode::GetPosition() const
{
    if (const auto component = m_registry->try_get<PositionComponent>(m_entity)) {
        return component->value;
    }
    // TODO: Warning
    static constexpr glm::vec3 zero(0, 0, 0);
    return zero;
}

void SceneNode::SetRotation(const glm::quat& rotation)
{
    if (const auto component = m_registry->try_get<OrientationComponent>(m_entity)) {
        component->value = rotation;
    }
    // TODO: Warning
}

const glm::quat& SceneNode::GetRotation() const
{
    if (const auto component = m_registry->try_get<OrientationComponent>(m_entity)) {
        return component->value;
    }
    static constexpr glm::quat identity(1, 0, 0, 0);
    return identity;
}

void SceneNode::SetScale(const glm::vec3& scale)
{
    if (const auto& component = m_registry->try_get<ScaleComponent>(m_entity)) {
        component->value = scale;
    }
    // TODO: Warning
}

const glm::vec3& SceneNode::GetScale() const
{
    if (const auto& component = m_registry->try_get<ScaleComponent>(m_entity)) {
        return component->value;
    }
    static constexpr glm::vec3 one(1, 1, 1);
    return one;
}

void SceneNode::LookAt(const glm::vec3& center, const glm::vec3& up)
{
    SetRotation(glm::quatLookAt(glm::normalize(center - GetPosition()), up));
}
