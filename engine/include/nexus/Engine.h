//
// Created by nutta on 8/16/2025.
//

#pragma once

#include "NxsDefine.h"
#include "core/LogDispatcher.h"
#include "core/task/TaskScheduler.h"
#include "graphics/GraphicsConfig.h"
#include "graphics/RenderSystem.h"
#include "graphics/MaterialManager.h"
#include "graphics/ModelManager.h"
#include "graphics/TextureManager.h"
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

        static void InitModules();

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

        NODISCARD Ref<TextureManager> GetTextureManager() const
        {
            NXS_ASSERT_MSG(m_textureManager != nullptr, "TextureManager hasn't been initialized");
            return m_textureManager;
        }
        NODISCARD Ref<ModelManager> GetModelManager() const
        {
            NXS_ASSERT_MSG(m_modelManager != nullptr, "ModelManager hasn't been initialized");
            return m_modelManager;
        }
        NODISCARD Ref<MaterialManager> GetMaterialManager() const
        {
            NXS_ASSERT_MSG(m_materialManager != nullptr, "MaterialManager hasn't been initialized");
            return m_materialManager;
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
        Ref<TextureManager> m_textureManager;
        Ref<ModelManager> m_modelManager;
        Ref<MaterialManager> m_materialManager;
        Ref<TaskScheduler> m_taskScheduler;
        Ref<SceneManager> m_sceneManager;
    };
}
