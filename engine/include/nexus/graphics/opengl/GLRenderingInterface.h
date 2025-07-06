//
// Created by nutta on 7/4/2025.
//

#pragma once

#include "NxsGL.h"
#include <nexus/graphics/RenderingInterface.h>

NXS_NAMESPACE
{
    class GLRenderingInterface final : public RenderingInterface
    {
    public:
        GLRenderingInterface(WindowContext window, const GraphicsConfig& config);
        ~GLRenderingInterface() override;
        void ClearColor(const Color4F& color) override;
        void ClearDepth(float depth) override;
        void ClearBuffer(const Color4F& color, float depth) override;
        void SwapBuffer() override;
        VertexBuffer* CreateVertexBuffer() override;
        IndexBuffer* CreateIndexBuffer(std::vector<uint32>&& indices, BufferUsage usage) override;
        void OnResize(uint32_t pixel_w, uint32_t pixel_h) override;

    protected:
        SDL_Window* m_window = nullptr;
        SDL_GLContext m_renderContext;
    };
}
