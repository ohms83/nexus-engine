#ifndef __NXS_GL_RENDER_SYSTEM_H__
#define __NXS_GL_RENDER_SYSTEM_H__

#include "graphics/RenderSystem.hpp"

NXS_NAMESPACE {
    class GLRenderSystem : public RenderSystem
    {
    public:
        GLRenderSystem();
        virtual ~GLRenderSystem();
    private:
    };
}

#endif // __NXS_GL_RENDER_SYSTEM_H__