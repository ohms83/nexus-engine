#ifndef __NXS_APPLICATION_H__
#define __NXS_APPLICATION_H__

#include "nxsMacros.h"
#include "graphics/RenderSystem.hpp"

#include <SDL2/SDL.h>

#include <string>
#include <memory>

NXS_NAMESPACE {
    class Application
    {
    public:
        Application();
        virtual ~Application();

        void init(const std::string& appName, uint32_t width, uint32_t height);
        void mainLoop();

    protected:
        virtual void onInit();
        virtual void onUpdate(float dt);
        virtual void onEvent(const SDL_Event& event);

        virtual RenderSystem* createRenderSystem() = 0;

    private:
        std::string _name;
        bool _running = true;
        uint32_t _screenWidth = 0;
        uint32_t _screenHeight = 0;
        SDL_Window* _window = NULL;
        SDL_Surface* _screenSurface = NULL;
        
        std::unique_ptr<RenderSystem> _renderSystem;
    };
}

#endif // __NXS_APPLICATION_H__