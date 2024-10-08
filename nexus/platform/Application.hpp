#ifndef __NXS_APPLICATION_H__
#define __NXS_APPLICATION_H__

#include "NxsDefine.h"
#include "graphics/RenderSystem.hpp"

#include <SDL2/SDL.h>

#include <string>
#include <memory>

NXS_NAMESPACE {
    class Application
    {
    public:
        /**
         * @c Application::Info contains information about how the application should be initialized.
         */
        struct Info
        {
            /// Application's name.
            std::string appName;
            uint32_t screenWidth;
            uint32_t screenHeight;
            /**
             * Must be either @c nexus::RENDER_SYSTEM_GL or @c nexus::RENDER_SYSTEM_VULKAN .
             * @see GraphicsConst.h
             */
            int renderSystem;
            /// Whether or not the application will launch in fullscreen mode.
            bool fullScreen;
        };

        Application();
        virtual ~Application();

        void init(const Info& info);
        void mainLoop();
        void draw();

    protected:
        virtual void onInit();
        virtual void onUpdate(float dt);
        virtual void onEvent(const SDL_Event& event);
        virtual void onDraw(RenderSystem& renderSystem);
        
        RenderSystem& getRenderSystem();

    private:
        Info _info;
        bool _running = true;
        SDL_Window* _window = NULL;
        
        std::unique_ptr<RenderSystem> _renderSystem;
    };
}

#endif // __NXS_APPLICATION_H__
