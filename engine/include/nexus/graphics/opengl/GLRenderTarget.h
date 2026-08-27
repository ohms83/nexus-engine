#pragma once

#include "nexus/NxsCommon.h"
#include "nexus/graphics/RenderTarget.h"
#include "nexus/graphics/TextureProxy.h"

NXS_NAMESPACE
{
    class GLRenderTarget : public RenderTarget
    {
    public:
        GLRenderTarget(uint32_t width, uint32_t height, PixelFormat format = PixelFormat::RGBA);
        ~GLRenderTarget() override;

        void Bind(RenderSystem& rs) override;
        void Unbind(RenderSystem& rs) override;

        NODISCARD uint32_t GetFBO() const { return m_fbo; }
        NODISCARD uint32_t GetWidth() const { return m_width; }
        NODISCARD uint32_t GetHeight() const { return m_height; }
        NODISCARD Ref<TextureProxy> GetColorTextureProxy() const { return m_colorTexture; }

    private:
        uint32_t m_fbo = 0;
        Ref<TextureProxy> m_colorTexture;
        Ref<TextureProxy> m_depthTexture;
        uint32_t m_width = 0;
        uint32_t m_height = 0;
        PixelFormat m_format = PixelFormat::RGBA;
        bool m_initialized = false;
    };
}
