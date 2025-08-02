#include "nexus/scene/Scene.h"

#include "ecs/component/scene/LightComponent.h"
#include "ecs/system/scene/SceneNodeTransformSystem.h"
#include "nexus/graphics/RenderSystem.h"
#include "nexus/core/LogDispatcher.h"

USING_NAMESPACE_NXS;

DEFINE_LOG(Scene);

Scene::Scene()
{
    m_ambient = m_registry.create();
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

void Scene::SetAmbient(const Color3F& color)
{
    m_registry.emplace<AmbientLightComponent>(m_ambient, color);
}
