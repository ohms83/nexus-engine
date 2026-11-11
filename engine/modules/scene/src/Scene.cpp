#include "nexus/scene/Scene.h"
#include "nexus/scene/component/LightComponent.h"
#include "nexus/scene/system/SceneNodeTransformSystem.h"

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

Dict<const ISceneView*, Array<SceneNode*>> Scene::GetVisibilitySets() const
{
    rmt_ScopedCPUSample(Scene_GetVisibilitySets, 0)
    Dict<const ISceneView*, Array<SceneNode*>> visibilitySets;
    VisitDescendants([&](const SceneNode& sceneNode) {
        if (!sceneNode.IsActive()) return;

        const auto view = D_CAST<const ISceneView*>(&sceneNode);
        if (!view) return;

        Array<SceneNode*> visibleNodes;
        {
            rmt_ScopedCPUSample(Scene_ComputeViewVisibility, 0)
            visibleNodes = view->GetVisibleNodes(*this);
        }
        visibilitySets.insert_or_assign(view, std::move(visibleNodes));
    });
    return visibilitySets;
}
