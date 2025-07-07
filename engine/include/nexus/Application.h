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

NXS_NAMESPACE
{
    struct ApplicationConfig
    {
        std::string title;
        GraphicsConfig graphicsConfig;
        bool fullscreen;
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

    protected:
        virtual bool Init_Internal() { return true; }
        virtual void Update() {}
        virtual void Render(RenderSystem* renderSystem) {}

        virtual void OnKeyDown(SDL_Keycode key);
        virtual void OnResize();

        void PollEvents(SDL_Event& e);

    protected:
        WindowContext m_window = nullptr;
        int32 m_screenWidth = 1280;
        int32 m_screenHeight = 960;
        int32 m_actualWidth = 1280;
        int32 m_actualHeight = 960;
        SDL_Keycode m_escapeKey = SDLK_ESCAPE;

    private:
        bool m_quit = false;
        RenderSystem* m_renderSystem = nullptr;
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
