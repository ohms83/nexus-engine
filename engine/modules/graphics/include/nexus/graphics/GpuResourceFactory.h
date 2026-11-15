#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/graphics/GraphicsConfig.h"

NXS_NAMESPACE
{
    class VertexBuffer;
    class IndexBuffer;
    class TextureProxy;
    class GpuProgram;

    class NXS_API IGpuResourceFactory
    {
    public:
        virtual ~IGpuResourceFactory() = default;

        NODISCARD virtual Ref<VertexBuffer> CreateVertexBuffer() const = 0;
        NODISCARD virtual Ref<IndexBuffer> CreateIndexBuffer() const = 0;
        NODISCARD virtual Ref<TextureProxy> CreateTextureProxy() const = 0;
        NODISCARD virtual Ref<GpuProgram> CreateGpuProgram() const = 0;
    };
}