#include "scene/Scene.h"
#include "scene/component/LightComponent.h"
#include "scene/system/SceneNodeTransformSystem.h"

#include "core/LogDispatcher.h"
#include "graphics/RenderSystem.h"

#include "Remotery.h"

USING_NAMESPACE_NXS;

DEFINE_LOG(Scene);

Scene::Scene()
    : SceneNode(std::make_shared<entt::registry>())
{
    Init();
}

Scene::Scene(const std::string& name)
    : SceneNode(std::make_shared<entt::registry>(), name)
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
    reflector.ChangeCategory("Rendering");
    reflector.VisitProperty("Ambient Color", typeid(Color3F), &Ambient());
}

void Scene::Init()
{
    AddSimulation(MoveNode);
    AddSimulation(RotateNode);

    AddComponent<AmbientLightComponent>();
}

void Scene::Render(RenderSystem &renderSystem)
{
    rmt_ScopedCPUSample(Scene_Render, 0)
    if (m_renderer) m_renderer->Render(renderSystem, *GetRegistry());
}

void Scene::SetRenderer(Ptr<ISceneRenderer> renderer)
{
    m_renderer = std::move(renderer);
}

Color3F& Scene::Ambient()
{
    return GetComponent<AmbientLightComponent>().color;
}

const Color3F& Scene::Ambient() const
{
    return GetComponent<AmbientLightComponent>().color;
}
