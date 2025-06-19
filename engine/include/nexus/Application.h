//
// Created by nutta on 6/18/2025.
//

#pragma once

#include "NxsDefine.h"
#include "graphics/RenderSystem.h"

#include <SDL3/SDL.h>

#include <concepts>
#include <string>
#include <cassert>

NXS_NAMESPACE
{
    struct ApplicationInitInfo
    {
        std::string title;
        int32_t screenWidth;
        int32_t screenHeight;
        GraphicsAPI renderAPI;
    };
    
    class Application
    {
    public:
        Application();
        virtual ~Application();

        bool Init(const ApplicationInitInfo& info);
        int BeginMainLoop();

        void RequestQuit();
        bool IsQuitRequested() const;

        RenderSystem& GetRenderSystem() const
        {
            assert(m_renderSystem);
            return *m_renderSystem;
        }

    protected:
        virtual bool Init_Internal() { return true; }
        virtual void Update() {}

        void PollEvents(SDL_Event& e);

    protected:
        SDL_Window* m_window = nullptr;
        int32_t m_screenWidth = 800;
        int32_t m_screenHeight = 600;

    private:
        bool m_quit = false;
        RenderSystem* m_renderSystem = nullptr;
    };

    template<typename T>
    requires std::derived_from<T, Application>
    int RunApplication(const ApplicationInitInfo& initInfo)
    {
        T application = T();
        application.Init(initInfo);
        return application.BeginMainLoop();
    }
}
