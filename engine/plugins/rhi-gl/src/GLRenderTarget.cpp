
#include "nexus/GLRenderTarget.h"
#include "nexus/GLTexture.h"
#include "nexus/GLRenderingInterface.h"
#include "nexus/graphics/RenderSystem.h"
#include "nexus/container/Array.h"

USING_NAMESPACE_NXS;

GLRenderTarget::GLRenderTarget(
    const uint32_t width,
    const uint32_t height,
    Ref<TextureProxy> colorTexture,
    Ref<TextureProxy> depthTexture,
    const PixelFormat format,
    const bool depthOnly)
    : m_colorTexture(std::move(colorTexture))
    , m_depthTexture(std::move(depthTexture))
    , m_width(width)
    , m_height(height)
    , m_format(format)
    , m_depthOnly(depthOnly)
{
}

GLRenderTarget::~GLRenderTarget()
{
    if (m_fbo)
    {
        CALL_GL_FUNC(glDeleteFramebuffers(1, &m_fbo));
        m_fbo = 0;
    }
}

void GLRenderTarget::Bind(RenderSystem& rs)
{
    auto renderInterface = rs.GetRenderInterface();
    if (!m_initialized)
    {
        if (m_colorTexture)
        {
            TextureDescription colorDesc;
            colorDesc.width = INT_CAST(m_width);
            colorDesc.height = INT_CAST(m_height);
            colorDesc.format = m_format;
            const size_t bytes = colorDesc.GetBufferSize();
            Array<uint8_t> empty(bytes, 0);
            m_colorTexture->Begin(colorDesc);
            m_colorTexture->LoadData(empty.data(), INT_CAST(bytes));
            m_colorTexture->End();
        }

        NXS_ASSERT_MSG(m_depthTexture, "Render target requires a depth texture.");
        TextureDescription dd;
        dd.width = INT_CAST(m_width);
        dd.height = INT_CAST(m_height);
        dd.channels = 1;
        dd.format = PixelFormat::Depth;
        dd.filterMin = TextureFilterMode::Nearest;
        dd.filterMag = TextureFilterMode::Nearest;
        dd.wrapModeS = TextureWrapMode::ClampToEdge;
        dd.wrapModeT = TextureWrapMode::ClampToEdge;
        m_depthTexture->Begin(dd);
        m_depthTexture->LoadData(nullptr, 0);
        m_depthTexture->End();

        CALL_GL_FUNC(glGenFramebuffers(1, &m_fbo));
        CALL_GL_FUNC(glBindFramebuffer(GL_FRAMEBUFFER, m_fbo));
        if (m_colorTexture)
        {
            CALL_GL_FUNC(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorTexture->GetHandle(), 0));
        }
        CALL_GL_FUNC(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture->GetHandle(), 0));
        if (m_depthOnly)
        {
            CALL_GL_FUNC(glDrawBuffer(GL_NONE));
            CALL_GL_FUNC(glReadBuffer(GL_NONE));
        }
        const auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        NXS_ASSERT(status == GL_FRAMEBUFFER_COMPLETE);
        CALL_GL_FUNC(glBindFramebuffer(GL_FRAMEBUFFER, 0));
        m_initialized = true;
    }

    CALL_GL_FUNC(glBindFramebuffer(GL_FRAMEBUFFER, m_fbo));
    // Setup viewport
    renderInterface->SetViewport(0, 0, INT_CAST(m_width), INT_CAST(m_height));
}

void GLRenderTarget::Unbind(RenderSystem& rs)
{
    auto renderInterface = rs.GetRenderInterface();
    CALL_GL_FUNC(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    // Restore viewport to screen size
    const auto& cfg = rs.GetGraphicsConfig();
    renderInterface->SetViewport(0, 0, cfg.screenWidth, cfg.screenHeight);
}
