#ifndef __NXS_GL_RENDER_SYSTEM_H__
#define __NXS_GL_RENDER_SYSTEM_H__

#include "NxsGL.h"
#include "graphics/RenderSystem.hpp"

#include <string>

NXS_NAMESPACE {
    class GLRenderSystem : public RenderSystem
    {
    public:
        GLRenderSystem();
        virtual ~GLRenderSystem();

        void init(
            std::string appName,
            uint32_t screenWidth,
            uint32_t screenHeight
        ) override;

        void setClearColor(const Color4F& color) override;

        void clear() override;
        void draw() override;
        void swapBuffer() override;

    private:
        SDL_GLContext _context;
        SDL_Window* _window = nullptr;
    };
}

#endif // __NXS_GL_RENDER_SYSTEM_H__
