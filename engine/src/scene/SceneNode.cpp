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
    if (m_node.active != activate)
    {
        m_node.active = activate;
        if (activate) OnActivate();
        else OnDeactivate();
    }
}

void SceneNode::AddChild(Ref<SceneNode> child)
{
    child->m_parent = this;
    m_children.push_back(child);
}

void SceneNode::RemoveChild(Ref<SceneNode> child)
{
    if (auto itr = std::ranges::find(m_children, child); itr != m_children.end())
    {
        child->m_parent = nullptr;
        m_children.erase(itr);
    }
}

void SceneNode::GetAllChildren(ChildList& childrenList) const
{
    for (auto child : m_children) childrenList.push_back(child);
}

void SceneNode::AddScript(Ref<Script> script)
{
    m_scripts.push_back(script);

    std::ranges::sort(m_scripts, std::ranges::greater{}, &Script::GetPriority);
}

void SceneNode::RemoveScript(Ref<Script> script)
{
    std::erase(m_scripts, script);
}

void SceneNode::Update(float dt)
{
    std::ranges::for_each(m_scripts, [dt](Ref<Script> script) {
        script->Update(dt);
    });

    Update_Internal(dt);
}
