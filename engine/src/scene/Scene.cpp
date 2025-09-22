#include "nexus/scene/Scene.h"

#include "ecs/component/scene/LightComponent.h"
#include "ecs/system/scene/SceneNodeTransformSystem.h"
#include "nexus/graphics/RenderSystem.h"
#include "nexus/core/LogDispatcher.h"

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

void Scene::Init()
{
    auto ambientEntt = m_registry.create();
    m_ambientComponent = &m_registry.emplace<AmbientLightComponent>(ambientEntt, Color3F::Red);

    AddSimulation(MoveNode);
    AddSimulation(RotateNode);
}

Ref<SceneNode> Scene::GetNode(const std::string& name)
{
    const auto node = std::ranges::find_if(m_children, [&name](const Ref<SceneNode>& n)
    {
        return n->GetName() == name;
    });
    return node == m_children.end() ? nullptr : *node;
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
    auto [begin, end] = std::ranges::remove_if(m_simulations, [id](const Simulation& simulation) {
        return simulation.id == id;
    });
    m_simulations.erase(begin, end);
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
