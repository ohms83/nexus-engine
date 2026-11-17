#pragma once

#include "GraphicsConst.h"
#include "TextureProxy.h"

NXS_NAMESPACE
{
    class RenderSystem;

    enum class RenderTargetType
    {
        /**
         * @brief Render to the swapchain (screen).
         */
        Swapchain,
        /**
         * @brief Render to an offscreen render target.
         */
        Offscreen
    };

    /**
     * @brief Represents a render target where the rendering output is directed.
     * 
     */
    class RenderTarget
    {
    public:
        virtual ~RenderTarget() = default;

        /**
         * @brief Binds the render target for rendering.
         */
        virtual void Bind(RenderSystem& rs) = 0;

        /**
         * @brief Unbinds the render target.
         */
        virtual void Unbind(RenderSystem& rs) = 0;

        NODISCARD virtual Ref<TextureProxy> GetDepthTextureProxy() const = 0;
    };
}