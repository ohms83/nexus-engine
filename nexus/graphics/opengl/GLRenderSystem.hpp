#ifndef __NXS_GL_RENDER_SYSTEM_H__
#define __NXS_GL_RENDER_SYSTEM_H__

#include "graphics/RenderSystem.hpp"

NXS_NAMESPACE {
    class GLRenderSystem : public RenderSystem
    {
    public:
        GLRenderSystem();
        virtual ~GLRenderSystem();

        void init(SDL_Window* window) override;
        int getSDLInitFlag() const override;

        void setClearColor(const Color& color) override;

        void clear() override;
        void draw() override;
        void swapBuffer() override;

    private:
        SDL_GLContext _context;
        SDL_Window* _window = nullptr;
    };
}

#endif // __NXS_GL_RENDER_SYSTEM_H__