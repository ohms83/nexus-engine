#include "scene/Scene.h"

#include "core/LogDispatcher.h"
#include "ecs/component/scene/LightComponent.h"
#include "ecs/system/scene/SceneNodeTransformSystem.h"
#include "graphics/RenderSystem.h"
#include "core/task/OneshotTask.h"
#include "Engine.h"

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

void Scene::Init()
{
    auto ambientEntt = m_registry.create();
    m_ambientComponent = &m_registry.emplace<AmbientLightComponent>(ambientEntt, Color3F::Red);

    AddSimulation(MoveNode);
    AddSimulation(RotateNode);
}

Ref<SceneNode> Scene::FindNode(const std::string& name)
{
    const auto node = std::ranges::find_if(m_children, [&name](const Ref<SceneNode>& n)
    {
        return n->GetName() == name;
    });

    if (node == m_children.end())
    {
        LOG_WARNING(LogScene, std::format("Can't find a scene node with name: {}", name));
        return nullptr;
    }
    return *node;
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

    auto taskScheduler = Engine::Instance().GetTaskScheduler();
    taskScheduler->ScheduleTask(std::make_shared<OneshotTask>([this, node]() {
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
    RemoveNode(FindNode(name));
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
    NXS_ASSERT(m_ambientComponent);
    return m_ambientComponent->color;
}

const Color3F& Scene::Ambient() const
{
    NXS_ASSERT(m_ambientComponent);
    return m_ambientComponent->color;
}
