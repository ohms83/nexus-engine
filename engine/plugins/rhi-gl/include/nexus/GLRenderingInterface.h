//
// Created by nutta on 7/4/2025.
//

#pragma once

#include "NxsGL.h"
#include "nexus/graphics/RenderingInterface.h"

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
        void SetFramebufferSRGB(bool enabled) override;
        NODISCARD bool IsFramebufferSRGBSupported() const override { return m_framebufferSRGBSupported; }

        void SetViewport(int32_t x, int32_t y, int32_t w, int32_t h) override;

        void OnResize(uint32_t pixel_w, uint32_t pixel_h) override;

        void DrawIndexed(const Ref<IndexBuffer> indexBuffer) override;
        void DrawIndexedInstanced(const Ref<IndexBuffer> indexBuffer, uint32_t instanceCount) override;

        void SetColorMask(const glm::bvec4& mask) override;
        void SetDepthWrite(bool mask) override;
        void SetStencilTest(bool enabled) override;
        void SetStencilOperation(StencilOperation operation) override;

        void SetDepthFunction(DepthFunction depthFunction) override;
        void SetBlendMode(BlendMode blendMode) override;
        void SetLineWidth(float width) override;
        void SetPolygonMode(PolygonMode mode) override;
        void SetCullMode(PolygonFacing face) override;
        void SetFrontFace(FrontFace face) override;

        void EnableDrawBuffer(DrawBuffer buffer) override;

        void DebugMarker(const std::string& message) override;

    protected:
        WindowContext m_window {};
        bool m_framebufferSRGBSupported = false;
    };
}
