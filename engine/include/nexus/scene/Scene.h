#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/core/Color.h"
#include "nexus/core/LogDispatcher.h"

#include "SceneNode.h"
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
        IMPLEMENT_REFLECTION(Scene);

        explicit Scene(const std::string& name);
        virtual ~Scene();

        virtual void OnEnter() {}
        virtual void OnExit() {}
        virtual void Render(RenderSystem& renderSystem) const;

        void SetRenderer(Ptr<SceneRenderer> renderer);

        Color3F& Ambient();
        NODISCARD const Color3F& Ambient() const;

    protected:
        // --- Rendering ---
        Ptr<SceneRenderer> m_renderer;
    };
}
