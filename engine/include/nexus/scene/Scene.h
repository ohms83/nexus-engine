#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/graphics/Color.h"
#include "nexus/core/LogDispatcher.h"

#include "SceneNode.h"
#include "SceneRenderer.h"
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

        Scene();
        explicit Scene(const std::string& name);
        virtual ~Scene();

        void AcceptReflector(IReflector& reflector) override;

        virtual void OnEnter() {}
        virtual void OnExit() {}
        virtual void Render(RenderSystem& renderSystem);

        void SetRenderer(Ptr<ISceneRenderer> renderer);

        Color3F& Ambient();
        NODISCARD const Color3F& Ambient() const;

    private:
        void Init();

    protected:
        // --- Rendering ---
        Ptr<ISceneRenderer> m_renderer;
    };
}
