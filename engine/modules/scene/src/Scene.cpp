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

void Scene::AddChild(Ref<SceneNode> child)
{
    Base::AddChild(child);

    if (m_partition) {
        m_partition->AddNode(child, child->GetBound());
        child->VisitDescendants([&](SceneNode& descendant) {
            m_partition->AddNode(descendant.GetSelf(), descendant.GetBound());
        });
    }
}

void Scene::RemoveChild(Ref<SceneNode> child, bool reparentDescendants)
{
    Base::RemoveChild(child, reparentDescendants);

    if (m_partition && child) {
        m_partition->RemoveNode(child->GetId());
        if (!reparentDescendants) {
            child->VisitDescendants([&](SceneNode& descendant) {
                m_partition->RemoveNode(descendant.GetId());
            });
        }
    }
}

void Scene::Render(RenderSystem &renderSystem) const
{
    rmt_ScopedCPUSample(Scene_Render, 0)
    if (m_renderer) m_renderer->Render(renderSystem, *this);
}

void Scene::SetRenderer(Ref<SceneRenderer> renderer)
{
    if (renderer && !renderer->IsInit()) {
        LOG_ERROR(LogScene, "Scene renderer hasn't yet been initialized!");
        return;
    }
    m_renderer = std::move(renderer);
}

void Scene::SetPartition(Ref<IScenePartition> partition)
{
    if (m_partition) {
        m_partition->RemoveAllNodes();
    }

    m_partition = partition;
    if (m_partition) {
        VisitDescendants([&](SceneNode& descendant) {
            m_partition->AddNode(descendant.GetSelf(), descendant.GetBound());
        });
    }
}

void Scene::RefreshPartition()
{
    if (!m_partition) return;

    m_partition->RemoveAllNodes();
    VisitDescendants([&](SceneNode& descendant) {
        m_partition->AddNode(descendant.GetSelf(), descendant.GetBound());
    });
}

Color3F& Scene::Ambient()
{
    return GetComponent<AmbientLightComponent>()->color;
}

const Color3F& Scene::Ambient() const
{
    return GetComponent<AmbientLightComponent>()->color;
}
