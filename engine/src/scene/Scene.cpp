#include "nexus/scene/Scene.h"

#include "ecs/component/scene/LightComponent.h"
#include "ecs/system/scene/SceneNodeTransformSystem.h"
#include "nexus/graphics/RenderSystem.h"
#include "nexus/core/LogDispatcher.h"

USING_NAMESPACE_NXS;

DEFINE_LOG(Scene);

Scene::Scene()
{
    auto ambientEntt = m_registry.create();
    m_ambientComponent = &m_registry.emplace<AmbientLightComponent>(ambientEntt, Color3F::Red);

    m_simulations.push_back(MoveNode);
    m_simulations.push_back(RotateNode);
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
    for (const auto& system : m_simulations)
    {
        system(GetRegistry(), dt);
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
