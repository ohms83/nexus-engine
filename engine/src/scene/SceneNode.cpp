#include <utility>

#include "scene/SceneNode.h"
#include "ecs/component/scene/SceneNodeComponent.h"
#include "core/LogDispatcher.h"

USING_NAMESPACE_NXS;

DEFINE_LOG(SceneNode);

static uint64_t s_runningId = 0;
const uint64_t SceneNode::InvalidID = 0;

SceneNode::SceneNode(entt::registry& registry)
    : Entity(registry)
    , m_id(++s_runningId)
{
    AddComponent<SceneNodeComponent>();
}

SceneNode::SceneNode(entt::registry& registry, std::string name)
    : Entity(registry)
    , m_id(++s_runningId)
{
    AddComponent<SceneNodeComponent>(name, true);
}

SceneNode::~SceneNode()
{
    m_id = InvalidID;
}

void SceneNode::Activate(const bool activate)
{
    if (IsActive() != activate)
    {
        GetComponent<SceneNodeComponent>().active = activate;
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

void SceneNode::GetAllDescendants(ChildList& childrenList, bool parentFirst) const
{
    for (auto child : m_children)
    {
        if (parentFirst) childrenList.push_back(child);
        else childrenList.insert(childrenList.begin(), child);

        child->GetAllDescendants(childrenList, parentFirst);
    }
}

void SceneNode::RemoveFromParent()
{
    if (!m_parent) return;

    std::erase_if(m_parent->m_children, [&](Ref<SceneNode> child) {
        return child.get() == this;
    });
    m_parent = nullptr;
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

    OnUpdate(dt);
}
