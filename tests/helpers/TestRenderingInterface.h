#pragma once

#include "nexus/NxsCommon.h"
#include "nexus/graphics/RenderingInterface.h"

USING_NAMESPACE_NXS;

class FakeRendering final : public RenderingInterface
{
public:
    FakeRendering() {}
    ~FakeRendering() override {}

    void ClearColor(const Color4F& color) override { }
    void ClearDepth(float depth) override { }
    void ClearBuffer(const Color4F& color, float depth) override { }
    void SwapBuffer() override { }
    void SetViewport(int32 x, int32 y, int32 w, int32 h) override { }
    VertexBuffer* CreateVertexBuffer() const override { return nullptr; }
    IndexBuffer* CreateIndexBuffer() const override { return nullptr; }
    GpuProgram* CreateGpuProgram() const override { return nullptr; }
    TextureProxy* CreateTexture() const override { return nullptr; }
    void OnResize(uint32_t pixel_w, uint32_t pixel_h) override { }
    void DrawIndexed(const Ref<IndexBuffer> indexBuffer) override { }
    void DrawIndexedInstanced(const Ref<IndexBuffer> indexBuffer, uint32 instanceCount) override { }
    void SetColorMask(const glm::bvec4& mask) override { }
    void SetDepthMask(bool mask) override { }
    void SetDepthFunction(DepthFunction depthFunction) override { }
    void SetLineWidth(float width) override { }
    void EnableDrawBuffer(DrawBuffer buffer) override { }
};
