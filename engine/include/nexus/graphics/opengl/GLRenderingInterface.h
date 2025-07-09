//
// Created by nutta on 7/4/2025.
//

#pragma once

#include "NxsGL.h"
#include <nexus/graphics/RenderingInterface.h>
#include "SDL3/SDL_video.h"

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

        void SetViewport(int32 x, int32 y, int32 w, int32 h) override;

        [[nodiscard]] VertexBuffer* CreateVertexBuffer() const override;
        [[nodiscard]] IndexBuffer* CreateIndexBuffer() const override;
        [[nodiscard]] Shader* CreateShader() const override;

        void OnResize(uint32_t pixel_w, uint32_t pixel_h) override;

    protected:
        void Draw_Internal(const RenderCommand& command) override;

    protected:
        SDL_Window* m_window = nullptr;
    };
}
