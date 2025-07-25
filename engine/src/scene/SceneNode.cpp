#include <utility>

#include "nexus/scene/SceneNode.h"

#include <format>

#include "ecs/scene/TransformComponent.h"
#include "scene/Scene.h"

USING_NAMESPACE_NXS;

SceneNode::SceneNode(entt::registry& registry)
    : m_registry(&registry)
{
    m_entity = registry.create();
}

SceneNode::SceneNode(entt::registry& registry, std::string  name)
    : m_name(std::move(name))
    , m_registry(&registry)
{
    m_entity = registry.create();
}

void SceneNode::Activate(const bool activate)
{
    m_active = activate;
    if (activate) OnActivate();
    else OnDeactivate();
}

void SceneNode::SetPosition(const glm::vec3& position)
{
    m_registry->get<PositionComponent>(m_entity).value = position;
}

const glm::vec3& SceneNode::GetPosition() const
{
    return m_registry->get<PositionComponent>(m_entity).value;
}

void SceneNode::SetRotation(const glm::quat& rotation)
{
    m_registry->get<RotationComponent>(m_entity).value = rotation;
}

const glm::quat& SceneNode::GetRotation() const
{
    return m_registry->get<RotationComponent>(m_entity).value;
}

void SceneNode::SetScale(const glm::vec3& scale)
{
    m_registry->get<ScaleComponent>(m_entity).value = scale;
}

const glm::vec3& SceneNode::GetScale() const
{
    return m_registry->get<ScaleComponent>(m_entity).value;
}

void SceneNode::LookAt(const glm::vec3& center, const glm::vec3& up)
{
    SetRotation(glm::quatLookAt(glm::normalize(center - GetPosition()), up));
}
