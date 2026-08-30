//
// Created by nutta on 8/16/2025.
//

#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/debug/LogDispatcher.h"
#include "nexus/task/TaskScheduler.h"
#include "graphics/GraphicsConfig.h"
#include "graphics/RenderSystem.h"
#include "scene/SceneManager.h"

NXS_NAMESPACE
{
    class Engine final
    {
    public:
        Engine() = default;
        ~Engine() = default;

        //! Initalize engine.
        MAYBE_UNUSED static Engine& Initialize(WindowContext window, const GraphicsConfig& graphicsConfig);
        //! Detroy engine's singletone and clean up.
        static void Destroy();
        //! Get engine's singleton instance.
        NODISCARD static Engine& Instance();

        static void BeginShutdown();

        NODISCARD Ref<RenderSystem> GetRenderSystem() const
        {
            NXS_ASSERT_MSG(m_renderSystem != nullptr, "Render System hasn't been initialized");
            return m_renderSystem;
        }

        NODISCARD Ref<RenderingInterface> GetRenderingInterface() const
        {
            NXS_ASSERT_MSG(m_renderSystem != nullptr, "Render System hasn't been initialized");
            return m_renderSystem->GetRenderInterface();
        }

        NODISCARD Ref<ResourceManager> GetResourceManager() const
        {
            NXS_ASSERT_MSG(m_resourceManager != nullptr, "ResourceManager hasn't been initialized");
            return m_resourceManager;
        }

        NODISCARD Ref<TaskScheduler> GetTaskScheduler() const
        {
            NXS_ASSERT_MSG(m_taskScheduler != nullptr, "TaskScheduler hasn't been initialized");
            return m_taskScheduler;
        }

        NODISCARD Ref<SceneManager> GetSceneManager() const
        {
            NXS_ASSERT_MSG(m_sceneManager != nullptr, "SceneManager hasn't been initialized");
            return m_sceneManager;
        }

    private:
        Ref<RenderSystem> m_renderSystem;
        Ref<ResourceManager> m_resourceManager;
        Ref<TaskScheduler> m_taskScheduler;
        Ref<SceneManager> m_sceneManager;

        static void InitModules();
        static void ShutdownModules();
    };
}
