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

    /**
     * @brief Creates graphics-API-specific GPU resources for the active RHI plugin.
     *
     * Implementations of this interface return resources backed by the active graphics API,
     * while callers use the engine's common resource interfaces without depending on the
     * underlying implementation.
     *
     * @see IRHIPlugin
     * @see GetGpuResourceFactory
     */
    class IGpuResourceFactory
    {
    public:
        virtual ~IGpuResourceFactory() = default;

        /**
         * @brief Creates an uninitialized vertex buffer.
         * @return A shared vertex-buffer resource for the active graphics API.
         */
        NODISCARD virtual Ref<VertexBuffer> CreateVertexBuffer() const = 0;

        /**
         * @brief Creates an uninitialized index buffer.
         * @return A shared index-buffer resource for the active graphics API.
         */
        NODISCARD virtual Ref<IndexBuffer> CreateIndexBuffer() const = 0;

        /**
         * @brief Creates a texture proxy from a texture description.
         * @param desc Dimensions, format, sampling, wrapping, mip, and color-space settings.
         * @return A shared texture resource configured for the active graphics API.
         *
         * The returned texture is ready to receive data. The description determines whether
         * it represents a two-dimensional or cube texture.
         */
        NODISCARD virtual Ref<TextureProxy> CreateTextureProxy(const TextureDescription& desc) const = 0;

        /**
         * @brief Creates an RGB cube texture with linear sRGB filtering and clamp-to-edge wrapping.
         * @param width Width of each cube face.
         * @param height Height of each cube face.
         * @return A shared cube-texture resource for the active graphics API.
         */
        NODISCARD virtual Ref<TextureProxy> CreateCubeTexture(int32_t width, int32_t height) const = 0;

        /**
         * @brief Creates an uninitialized GPU shader program.
         * @return A shared GPU-program resource for the active graphics API.
         */
        NODISCARD virtual Ref<GpuProgram> CreateGpuProgram() const = 0;

        /**
         * @brief Creates an offscreen render target.
         * @param desc Dimensions and pixel format for the render target attachments.
         * @return A shared render target containing the appropriate color and depth attachments.
         *
         * A depth-only description creates a depth texture without a color attachment.
         */
        NODISCARD virtual Ref<RenderTarget> CreateRenderTarget(const TextureDescription& desc) const = 0;
    };

    /**
     * @brief Gets the GPU resource factory owned by the initialized RHI plugin.
     * @return Pointer to the active resource factory.
     *
     * This function requires the RHI plugin to have been initialized. The returned pointer
     * is owned by the active plugin and must not be deleted by the caller.
     */
    NODISCARD IGpuResourceFactory* GetGpuResourceFactory();
}