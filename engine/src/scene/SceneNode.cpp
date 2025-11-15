#include <utility>

#include "scene/SceneNode.h"
#include "core/LogDispatcher.h"
#include "core/task/OneshotTask.h"

USING_NAMESPACE_NXS;

DEFINE_LOG(SceneNode);

static uint64_t s_runningId = 0;
const uint64_t SceneNode::InvalidID = 0;

SceneNode::SceneNode(Ref<entt::registry> registry)
    : Entity(registry)
    , m_id(++s_runningId)
{
    AddComponent<SceneNodeComponent>();
}

SceneNode::SceneNode(Ref<entt::registry> registry, std::string name)
    : Entity(registry)
    , m_id(++s_runningId)
{
    AddComponent<SceneNodeComponent>(name, true);
}

SceneNode::~SceneNode()
{
    m_id = InvalidID;
    OnDestroy();
}

void SceneNode::Destroy()
{
    // TODO: With how the scene nodes are passed around as shared pointers, there's no
    // effective way to implement this yet. We need to implement a new memory management first.
    NXS_ASSERT(false);
}

void SceneNode::AcceptReflector(IReflector& reflector)
{
    reflector.ChangeCategory("Properties");

    auto& comp = GetComponent<SceneNodeComponent>();
    reflector.VisitPropertyWithFeedback("Name", typeid(std::string), (void*)(comp.name.c_str()), [&comp](void* newValue) {
        comp.name = CAST<const char*>(newValue);
    });
    reflector.VisitProperty("Active", typeid(bool), (void*)&comp.active);
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

void SceneNode::RemoveChild(Ref<SceneNode> node)
{
    if (IsShuttingDown() || !node) return;
 
    if (!m_scheduler)
    {
        LOG_WARNING(LogSceneNode, std::format("Unable to remove the node={}. Reason=The task scheduler is invalid.", node->GetName()));
        return;
    }

    m_scheduler->ScheduleTask(std::make_shared<OneshotTask>([this, node]() {
        SceneNode::ChildList nodeList;
        node->GetAllChildren(nodeList);

        for (auto descendant : nodeList)
        {
            descendant->RemoveFromParent();
            AddChild(descendant);
        }

        std::erase(m_children, node);
    }), TaskScheduler::UpdatePhase::PostUpdate);
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

Ref<SceneNode> SceneNode::FindNode(const SceneNode::Id id)
{
    if (IsShuttingDown()) return nullptr;

    for (const auto child : m_children)
    {
        if (child->GetId() == id) return child;
        if (auto node = child->FindNode(id); node != nullptr) return node;
    }

    return nullptr;
}

Ref<SceneNode> SceneNode::FindNodeWithName(const std::string& name)
{
    if (IsShuttingDown()) return nullptr;

    for (const auto child : m_children)
    {
        if (child->GetName() == name) return child;
        if (auto node = child->FindNodeWithName(name); node != nullptr) return node;
    }

    return nullptr;
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

    auto& registry = *GetRegistry();
    std::ranges::for_each(m_simulations, [dt, &registry](Simulation& sim) {
        sim.system(registry, dt);
    });
    
    std::ranges::for_each(m_children, [dt](Ref<SceneNode> node) {
        node->Update(dt);
    });

    OnUpdate(dt);
}

uint32_t SceneNode::AddSimulation(ECS::SimulationSystem system)
{
    static uint32_t runningNumber = 0;
    m_simulations.push_back({ runningNumber, system });
    return runningNumber++;
}

void SceneNode::RemoveSimulation(uint32_t id)
{
    if (auto itr = std::ranges::find(m_simulations, id, &Simulation::id); itr != m_simulations.end()) {
        m_simulations.erase(itr);
    }
}
