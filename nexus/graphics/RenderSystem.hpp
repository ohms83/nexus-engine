#ifndef __NXS_RENDER_SYSTEM_H__
#define __NXS_RENDER_SYSTEM_H__

#include "NxsMacros.h"
#include "Color.hpp"

#include <string>
#include <SDL2/SDL.h>

NXS_NAMESPACE {
    class RenderSystem
    {
    public:
        RenderSystem();
        virtual ~RenderSystem();

        virtual void init(
            std::string appName,
            uint32_t screenWidth,
            uint32_t screenHeight
        ) = 0;

        virtual void clear() = 0;
        virtual void swapBuffer() = 0;

        /* --- Pipline related functions --- */
        // TODO: Move all of these to RenderPipeline class.
        virtual void draw() = 0;
        const Color4F& getClearColor() const;
        virtual void setClearColor(const Color4F& color);
        /* --- Pipline related functions --- */

        static RenderSystem* create(int renderSystem);
    
    protected:
        Color4F _clearColor = COLOR4F_BLACK;
    private:
    };
}

#endif // __NXS_RENDER_SYSTEM_H__
