#ifndef __NXS_RENDER_SYSTEM_H__
#define __NXS_RENDER_SYSTEM_H__

#include "NxsDefine.h"
#include "Color.hpp"
#include "RenderCommand.hpp"

#include <string>
#include <SDL2/SDL.h>
#include <vector>

NXS_NAMESPACE {
    class VertexBuffer;
    class Shader;
    class RenderCommand;
    
    /**
     * @c RenderSystem acts as a proxy between application and graphics API.
     * Application doesn't need to know about which API it's using and only needs to communicate with the
     * underlying graphics hardware using @c Rendersystem 's interfaces. This adds more degree of flexibility
     * and, in turn, makes the codes more portable.
     */
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
        
        /// Initialize @c ImGui library for the underlying graphics API.
        virtual void initGui();
        virtual void beginDrawGui();
        virtual void endDrawGui();
        
        virtual void beginDraw();
        virtual void draw();
        virtual void endDraw();
        
        const Color4F& getClearColor() const;
        virtual void setClearColor(const Color4F& color);
        
        virtual VertexBuffer* createVertexBuffer() = 0;
        virtual Shader* createShader() = 0;
        virtual RenderCommand* createCommand() = 0;
        
        void registerCommand(const RenderCommand* command);

        static RenderSystem* create(int renderSystem);
        
        void* const getRenderContext() const {
            return _renderContext;
        }
        
        SDL_Window* getWindow() const {
            return _window;
        }
        
    protected:
        virtual void clear() = 0;
        virtual void swapBuffer() = 0;
    
    protected:
        void* _renderContext = nullptr;
        SDL_Window* _window = nullptr;
        
        Color4F _clearColor = COLOR4F_BLACK;
        std::vector<const RenderCommand*> _commandPools;
    };
}

#endif // __NXS_RENDER_SYSTEM_H__
