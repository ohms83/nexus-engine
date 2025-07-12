//
// Created by nutta on 6/18/2025.
//

#pragma once

#include "NxsDefine.h"
#include <SDL3/SDL.h>

#include <concepts>
#include <string>
#include <cassert>

#include <nexus/graphics/RenderSystem.h>
#include <glm/glm.hpp>

#include "core/Timer.h"
#include "editor/Editor.h"

NXS_NAMESPACE
{
    struct ApplicationConfig
    {
        std::string title;
        GraphicsConfig graphicsConfig;
        bool fullscreen = false;
        bool editMode = true;
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
        [[nodiscard]] bool IsQuitRequested() const;

        [[nodiscard]] WindowContext GetWindowContext() const;

        [[nodiscard]] RenderSystem& GetRenderSystem() const
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
        const glm::ivec2& GetScreenSize() const
        {
            return m_screenSize;
        }

        /**
         * Actual screen size in pixel. The value might be higher than screen size in case of
         * the high dpi display.
         * @return The actual screen size in pixel.
         */
        const glm::ivec2& GetActualSize() const
        {
            return m_actualSize;
        }

        //! Get the duration between this and the previous frame in seconds.
        float GetDeltaTime() const
        {
            return m_deltaTime;
        }

    protected:
        virtual bool Init_Internal() { return true; }
        virtual void Update() {}
        virtual void Render(RenderSystem& renderSystem) {}

        virtual void OnKeyDown(SDL_Keycode key);
        virtual void OnResize(const glm::ivec2& screenSize, const glm::ivec2& actualSize);

        virtual void UpdateEditor() {}
        virtual void RenderEditor(const RenderSystem& renderSystem) {}

        void PollEvents(SDL_Event& e);

    protected:
        WindowContext m_window = nullptr;
        glm::ivec2 m_screenSize = glm::ivec2(1280, 960);
        glm::ivec2 m_actualSize = glm::ivec2(1280, 960);
        SDL_Keycode m_escapeKey = SDLK_ESCAPE;

    private:
        bool m_quit = false;
        RenderSystem* m_renderSystem = nullptr;
        std::unique_ptr<Editor> m_editor;

        Timer m_timer;
        float m_deltaTime = 0.0f;
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
