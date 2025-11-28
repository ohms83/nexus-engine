#pragma once

#include "GraphicsConst.h"

NXS_NAMESPACE
{
    class RenderSystem;

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
    };
}