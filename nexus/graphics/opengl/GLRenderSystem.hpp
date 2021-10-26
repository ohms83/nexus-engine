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
        
        void initGui() override;
        void beginDrawGui() override;
        void endDrawGui() override;
        
        VertexBuffer* createVertexBuffer() override;
        Shader* createShader() override;

        void setClearColor(const Color4F& color) override;

        void clear() override;
        void swapBuffer() override;
        
    protected:
        void useShader(std::shared_ptr<const Shader> shader) const override;
        void execute(std::shared_ptr<const RenderCommand> command) const override;
    };
}

#endif // __NXS_GL_RENDER_SYSTEM_H__
