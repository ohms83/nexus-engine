//
// Created by nutta on 7/4/2025.
//

#pragma once

#include "NxsGL.h"
#include "nexus/graphics/RenderingInterface.h"
#include "SDL3/SDL_video.h"

NXS_NAMESPACE
{
    class GLRenderingInterface final : public RenderingInterface
    {
        NXS_INHERIT(RenderingInterface)
    public:
        GLRenderingInterface(WindowContext window, const GraphicsConfig& config);
        ~GLRenderingInterface() override;
        void ClearColor(const Color4F& color) override;
        void ClearDepth(float depth) override;
        void ClearBuffer(const Color4F& color, float depth) override;

        void SwapBuffer() override;

        void SetViewport(int32 x, int32 y, int32 w, int32 h) override;

        NODISCARD VertexBuffer* CreateVertexBuffer() const override;
        NODISCARD IndexBuffer* CreateIndexBuffer() const override;
        NODISCARD GpuProgram* CreateGpuProgram() const override;
        NODISCARD TextureProxy* CreateTexture() const override;

        void OnResize(uint32_t pixel_w, uint32_t pixel_h) override;

        void DrawIndexed(const Ref<IndexBuffer> indexBuffer) override;
        void DrawIndexedInstanced(const Ref<IndexBuffer> indexBuffer, uint32 instanceCount) override;

        void SetColorMask(const glm::bvec4& mask) override;
        void SetDepthMask(bool mask) override;

        void SetDepthFunction(DepthFunction depthFunction) override;
        void SetLineWidth(float width) override;
        void SetPolygonMode(PolygonMode mode) override;
        void SetCullMode(PolygonFacing face) override;
        void SetFrontFace(FrontFace face) override;

        void EnableDrawBuffer(DrawBuffer buffer) override;

        void DebugMarker(const std::string& message) override;

    protected:
        SDL_Window* m_window = nullptr;
    };
}
