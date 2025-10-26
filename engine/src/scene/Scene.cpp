#include "scene/Scene.h"
#include "scene/component/LightComponent.h"
#include "scene/system/SceneNodeTransformSystem.h"

#include "core/LogDispatcher.h"
#include "core/task/OneshotTask.h"
#include "graphics/RenderSystem.h"

#include "Remotery.h"

#include <ranges>
#include <algorithm>

USING_NAMESPACE_NXS;

DEFINE_LOG(Scene);

Scene::Scene()
{
    Init();
}

Scene::Scene(const std::string& name)
    : m_name(name)
{
    Init();
}

Scene::~Scene()
{
    m_children.clear();
    m_simulations.clear();
}

void Scene::AcceptReflector(IReflector& reflector)
{
    reflector.ChangeCatetory("Rendering");
    reflector.VisitProperty("Ambient Color", typeid(Color3F), &Ambient());
}

void Scene::Init()
{
    m_entity = m_registry.create();
    m_registry.emplace<AmbientLightComponent>(m_entity, Color3F::Gray);

    AddSimulation(MoveNode);
    AddSimulation(RotateNode);
}

Ref<SceneNode> Scene::FindNode(SceneNode::Id id)
{
    if (IsShuttingDown()) return nullptr;

    const auto node = std::ranges::find_if(m_children, [&id](const Ref<SceneNode>& n)
    {
        return n->GetId() == id;
    });

    return node != m_children.end() ? *node : nullptr;
}

Ref<SceneNode> Scene::FindNodeWithName(const std::string& name)
{
    if (IsShuttingDown()) return nullptr;

    const auto node = std::ranges::find_if(m_children, [&name](const Ref<SceneNode>& n)
    {
        return n->GetName() == name;
    });

    return node != m_children.end() ? *node : nullptr;
}

void Scene::GetAllRootNodes(SceneNode::ChildList& nodeList) const
{
    auto isRoot = [](Ref<const SceneNode> child) {
        return child->GetParent() == nullptr;
    };
    std::ranges::for_each(m_children | std::views::filter(isRoot), [&nodeList](auto node) {
        nodeList.push_back(node);
    });
}

void Scene::RemoveNode(Ref<SceneNode> node)
{
    if (IsShuttingDown() || !node) return;

    if (!node)
    {
        LOG_WARNING(LogScene, std::format("Invalid scene node."));
        return;
    }
 
    if (!m_scheduler)
    {
        LOG_WARNING(LogScene, std::format("Unable to remove the node={}. Reason=The task scheduler is invalid.", node->GetName()));
        return;
    }

    m_scheduler->ScheduleTask(std::make_shared<OneshotTask>([this, node]() {
        SceneNode::ChildList nodeList;
        // Sort the list in descendants to parents order.
        node->GetAllDescendants(nodeList, false);

        for (auto node : nodeList)
        {
            node->OnDestroy();
            node->RemoveFromParent();
        }

        nodeList.push_back(node);
        std::erase_if(m_children, [&](Ref<SceneNode> node) {
            return std::ranges::find(nodeList, node) != nodeList.end();
        });
    }), TaskScheduler::UpdatePhase::PostUpdate);
}

void Scene::RemoveNodeByName(const std::string& name)
{
    auto node = FindNodeWithName(name); 
    if (node == nullptr)
    {
        LOG_WARNING(LogScene, std::format("Can't find a node with name={}", name));
        return;
    }
    RemoveNode(node);
}

void Scene::Update(float dt)
{
    for (const auto& sim : m_simulations)
    {
        sim.system(GetRegistry(), dt);
    }
    for (auto& node : m_children)
    {
        node->Update(dt);
    }
}

void Scene::Render(RenderSystem &renderSystem)
{
    rmt_ScopedCPUSample(Scene_Render, 0)
    if (m_renderer) m_renderer->Render(renderSystem, GetRegistry());
}

void Scene::SetRenderer(Ptr<ISceneRenderer> renderer)
{
    m_renderer = std::move(renderer);
}

uint32_t Scene::AddSimulation(ECS::SimulationSystem system)
{
    static uint32_t runningNumber = 0;
    m_simulations.push_back({ runningNumber, system });
    return runningNumber++;
}

void Scene::RemoveSimulation(uint32_t id)
{
    if (auto itr = std::ranges::find(m_simulations, id, &Simulation::id); itr != m_simulations.end()) {
        m_simulations.erase(itr);
    }
}

Color3F& Scene::Ambient()
{
    return m_registry.get<AmbientLightComponent>(m_entity).color;
}

const Color3F& Scene::Ambient() const
{
    return m_registry.get<AmbientLightComponent>(m_entity).color;
}
