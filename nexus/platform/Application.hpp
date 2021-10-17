#pragma once
#include "nxsMacros.h"

#include <SDL2/SDL.h>

#include <string>

NXS_NAMESPACE {
    class Application
    {
    public:
        Application(const std::string& appName, uint32_t width, uint32_t height);
        virtual ~Application();

        void mainLoop();

    protected:
        virtual void onUpdate(float dt);
        virtual void onEvent(const SDL_Event& event);

    private:
        std::string _name;
        bool _running = true;
        uint32_t _screenWidth = 0;
        uint32_t _screenHeight = 0;
        SDL_Window* _window = NULL;
        SDL_Surface* _screenSurface = NULL;
    };
}