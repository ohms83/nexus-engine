#ifndef __NXS_RENDER_SYSTEM_H__
#define __NXS_RENDER_SYSTEM_H__

#include "nxsMacros.h"

#include "Color.hpp"

#include <SDL2/SDL.h>

NXS_NAMESPACE {
    class RenderSystem
    {
    public:
        RenderSystem();
        virtual ~RenderSystem();

        virtual void init(SDL_Window* window) = 0;
        virtual int getSDLInitFlag() const = 0;

        /* --- Pipline related functions --- */
        // TODO: Move all of these to RenderPipeline class.
        virtual void clear() = 0;
        virtual void draw() = 0;
        virtual void swapBuffer() = 0;

        const Color& getClearColor() const;
        virtual void setClearColor(const Color& color);
        /* --- Pipline related functions --- */

        static RenderSystem* create(int renderSystem);
    
    protected:
        Color _clearColor = Color::BLACK;
    private:
    };
}

#endif // __NXS_RENDER_SYSTEM_H__