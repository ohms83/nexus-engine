#pragma once

#include "nexus/NxsDefine.h"

NXS_NAMESPACE
{
    class VertexBuffer;
    class IndexBuffer;
    class TextureProxy;
    class GpuProgram;
    class RenderTarget;
    struct TextureDescription;

    class IGpuResourceFactory
    {
    public:
        virtual ~IGpuResourceFactory() = default;

        NODISCARD virtual Ref<VertexBuffer> CreateVertexBuffer() const = 0;
        NODISCARD virtual Ref<IndexBuffer> CreateIndexBuffer() const = 0;
        NODISCARD virtual Ref<TextureProxy> CreateTextureProxy(const TextureDescription& desc) const = 0;
        NODISCARD virtual Ref<TextureProxy> CreateCubeTexture(int32_t width, int32_t height) const = 0;
        NODISCARD virtual Ref<GpuProgram> CreateGpuProgram() const = 0;
        NODISCARD virtual Ref<RenderTarget> CreateRenderTarget(const TextureDescription& desc) const = 0;
    };

    NODISCARD IGpuResourceFactory* GetGpuResourceFactory();
}