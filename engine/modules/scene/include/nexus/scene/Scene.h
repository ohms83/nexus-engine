#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/core/Color.h"
#include "nexus/debug/LogDispatcher.h"

#include "SceneNode.h"
#include "ScenePartition.h"
#include "renderer/SceneRenderer.h"
#include "component/LightComponent.h"

DECLARE_LOG_EXTERN(Scene);

NXS_NAMESPACE
{
    class RenderSystem;
    class TaskScheduler;

    class Scene : public SceneNode
    {
    public:
        using Base = SceneNode;

        IMPLEMENT_REFLECTION(Scene);

        explicit Scene(const std::string& name);
        virtual ~Scene() override;

        void AddChild(Ref<SceneNode> child) override;
        void RemoveChild(Ref<SceneNode> child, bool reparentDescendants = true) override;

        virtual void OnEnter() {}
        virtual void OnExit() {}
        virtual void Render(RenderSystem& renderSystem) const;

        void SetRenderer(Ref<SceneRenderer> renderer);
        void SetPartition(Ref<IScenePartition> partition);
        void RefreshPartition();
        Ref<IScenePartition> GetPartition() const { return m_partition; }

        Color3F& Ambient();
        NODISCARD const Color3F& Ambient() const;

        Dict<const ISceneView*, Array<SceneNode*>> GetVisibilitySets() const;

    protected:
        // --- Rendering ---
        Ref<SceneRenderer> m_renderer;
        Ref<IScenePartition> m_partition;
    };
}
