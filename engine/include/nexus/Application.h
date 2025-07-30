//
// Created by nutta on 6/18/2025.
//

#pragma once

#include "NxsDefine.h"
#include <SDL3/SDL.h>

#include <concepts>
#include <string>
#include <cassert>
#include <filesystem>

#include <nexus/graphics/RenderSystem.h>
#include <glm/glm.hpp>

#include "core/Timer.h"
#include "editor/Editor.h"
#include "resource/Mesh.h"
#include "resource/Texture.h"
#include "scene/Scene.h"

#define PURGE_UNUSED_RESOURCES(Manager) do { \
    LOG_INFO(LogResource, "Purge unused resources. Class="#Manager); \
    Get##Manager().PurgeUnused(); \
} while(0);

NXS_NAMESPACE
{
    struct ApplicationConfig
    {
        std::string title;
        GraphicsConfig graphicsConfig;
        bool fullscreen = false;
        bool resizable = false;
        bool editMode = true;
        bool maximize = true;
        int32 quitKey = SDLK_ESCAPE;
    };
    
    class Application
    {
    public:
        Application() = default;
        virtual ~Application();

        bool Init(const ApplicationConfig& info);
        int BeginMainLoop();

        void RequestQuit();
        NODISCARD bool IsQuitRequested() const;

        NODISCARD WindowContext GetWindowContext() const;

        NODISCARD RenderSystem& GetRenderSystem() const
        {
            assert(m_renderSystem);
            // ReSharper disable once CppDFANullDereference
            return *m_renderSystem;
        }

        /**
         * Get the desired screen size. This is the same value requested during application
         * initialization.
         * @return The desired screen size in pixels.
         */
        NODISCARD const glm::ivec2& GetScreenSize() const
        {
            return m_screenSize;
        }

        /**
         * Actual screen size in pixel. The value might be higher than screen size in case of
         * the high dpi display.
         * @return The actual screen size in pixel.
         */
        NODISCARD const glm::ivec2& GetActualSize() const
        {
            return m_actualSize;
        }

        //! Get the time since the begining of the application in seconds
        NODISCARD static float GetTimeSinceStart()
        {
            return FLOAT_CAST(SDL_GetTicks()) / 1000.0f;
        }

        //! Get the duration between this and the previous frame in seconds.
        NODISCARD float GetDeltaTime() const
        {
            return m_deltaTime;
        }

        NODISCARD MeshManager& GetMeshManager() const
        {
            NXS_ASSERT(m_meshManager);
            return *m_meshManager.get();
        }

        NODISCARD TextureManager& GetTextureManager() const
        {
            NXS_ASSERT(m_textureManager);
            return *m_textureManager.get();
        }

        MAYBE_UNUSED Ref<Scene> ChangeScene(const Ref<Scene>& scene);
        NODISCARD Ref<Scene> GetCurrentScene() const;

        NODISCARD std::string GetAssetPath(const std::string& path) const
        {
            return (std::filesystem::path(GetBaseAssetPath()) / path).string();
        }

    protected:
        virtual bool Init_Internal() { return true; }
        virtual void Update();
        virtual void Render(RenderSystem& renderSystem);

        virtual void OnEvent(const SDL_Event& e);
        virtual void OnFocusLost();
        virtual void OnFocusGain();
        virtual void OnKeyDown(SDL_Keycode key);
        virtual void OnKeyUp(SDL_Keycode key);
        virtual void OnMouseDown(int32 buttonId, float x, float y);
        virtual void OnMouseUp(int32 buttonId, float x, float y);
        virtual void OnMouseMove(float x, float y);
        virtual void OnResize(const glm::ivec2& screenSize, const glm::ivec2& actualSize);

        virtual void DrawUI() {}

        void PollEvents(SDL_Event& e);

        NODISCARD virtual std::string GetBaseAssetPath() const { return NXS_ASSETS_DIR; }

    private:
        void InitImGui() const;
        void BeginDrawUI();
        void EndDrawUI();
        void DestroyImGui();

    protected:
        WindowContext m_window = nullptr;
        glm::ivec2 m_screenSize = glm::ivec2(1280, 960);
        glm::ivec2 m_actualSize = glm::ivec2(1280, 960);
        SDL_Keycode m_escapeKey = SDLK_ESCAPE;

    private:
        bool m_quit = false;
        Ptr<RenderSystem> m_renderSystem;
        Ptr<Editor> m_editor;

        Ref<Timer> m_timer;
        float m_deltaTime = 0.0f;

        Ptr<MeshManager> m_meshManager;
        Ptr<TextureManager> m_textureManager;
        Ref<Scene> m_currentScene;
    };

    template<typename T>
    requires std::derived_from<T, Application>
    int RunApplication(const ApplicationConfig& initInfo)
    {
        T application = T();
        application.Init(initInfo);
        return application.BeginMainLoop();
    }
}
