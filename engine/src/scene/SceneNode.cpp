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

SceneNode3D::SceneNode3D(entt::registry &registry)
    : SceneNode(registry)
    , m_position(AddComponent<PositionComponent>())
    , m_orient(AddComponent<OrientationComponent>())
    , m_scale(AddComponent<ScaleComponent>())
{
}

SceneNode3D::SceneNode3D(entt::registry &registry, std::string name)
    : SceneNode(registry, name)
    , m_position(AddComponent<PositionComponent>())
    , m_orient(AddComponent<OrientationComponent>())
    , m_scale(AddComponent<ScaleComponent>())
{
}

void SceneNode3D::LookAt(const glm::vec3& center, const glm::vec3& up)
{
    m_orient.value = glm::quatLookAt(glm::normalize(center - m_position.value), up);
}
