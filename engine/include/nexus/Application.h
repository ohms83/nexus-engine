//
// Created by nutta on 6/18/2025.
//

#pragma once

#include <concepts>
#include <NxsDefine.h>
#include "SDL3/SDL.h"

NXS_NAMESPACE
{
    class Application
    {
    public:
        Application();
        virtual ~Application();

        bool Init();
        int BeginMainLoop();

        void RequestQuit();
        bool IsQuitRequested() const;

    protected:
        virtual bool Init_Internal() { return true; }
        virtual void Update() {}

        void PollEvents(SDL_Event& e);

    protected:
        SDL_Window* m_window = nullptr;

    private:
        bool m_quit = false;
    };

    template<typename T>
    requires std::derived_from<T, Application>
    int RunApplication()
    {
        T application = T();
        application.Init();
        return application.BeginMainLoop();
    }
}
