#include "nexus/scene/Scene.h"

#include "ecs/scene/LightComponent.h"
#include "nexus/graphics/RenderSystem.h"
#include "nexus/core/Logger.h"

USING_NAMESPACE_NXS;

DEFINE_LOG(Scene);

Scene::Scene()
{
    m_ambient = m_registry.create();
}

void Scene::Update()
{
    for (auto& node : m_children)
    {
        node->Update();
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
