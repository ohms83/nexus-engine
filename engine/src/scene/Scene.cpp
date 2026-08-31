#include "scene/Scene.h"
#include "scene/component/LightComponent.h"
#include "scene/system/SceneNodeTransformSystem.h"

#include "nexus/debug/LogDispatcher.h"
#include "nexus/graphics/RenderSystem.h"

#include "Remotery.h"

USING_NAMESPACE_NXS;

DEFINE_LOG(Scene);

Scene::Scene(const std::string& name)
    : SceneNode(std::make_shared<entt::registry>(), name)
{
    AddSimulation(MoveNode);
    AddSimulation(RotateNode);

    AddComponent<AmbientLightComponent>();
}

Scene::~Scene()
{
    m_children.clear();
    m_simulations.clear();
}

void Scene::Render(RenderSystem &renderSystem) const
{
    rmt_ScopedCPUSample(Scene_Render, 0)
    if (m_renderer) m_renderer->Render(renderSystem, *this);
}

void Scene::SetRenderer(Ref<SceneRenderer> renderer)
{
    m_renderer = renderer;
}

Color3F& Scene::Ambient()
{
    return GetComponent<AmbientLightComponent>()->color;
}

const Color3F& Scene::Ambient() const
{
    return GetComponent<AmbientLightComponent>()->color;
}
