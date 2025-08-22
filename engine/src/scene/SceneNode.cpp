#include <utility>

#include "scene/SceneNode.h"
#include "ecs/component/scene/SceneNodeComponent.h"

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
