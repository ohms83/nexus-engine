#pragma once

#include "nexus/NxsCommon.h"
#include "nexus/graphics/RenderTarget.h"
#include "nexus/graphics/TextureProxy.h"

NXS_NAMESPACE
{
    class GLRenderTarget : public RenderTarget
    {
    public:
        GLRenderTarget(uint32 width, uint32 height, PixelFormat format = PixelFormat::RGBA);
        ~GLRenderTarget() override;

        void Bind(RenderSystem& rs) override;
        void Unbind(RenderSystem& rs) override;

        NODISCARD uint32 GetFBO() const { return m_fbo; }
        NODISCARD uint32 GetWidth() const { return m_width; }
        NODISCARD uint32 GetHeight() const { return m_height; }

    private:
        uint32 m_fbo = 0;
        Ref<TextureProxy> m_colorTexture;
        Ref<TextureProxy> m_depthTexture;
        uint32 m_width = 0;
        uint32 m_height = 0;
        PixelFormat m_format = PixelFormat::RGBA;
        bool m_initialized = false;
    };
}
