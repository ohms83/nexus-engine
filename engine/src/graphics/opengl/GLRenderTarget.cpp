
#include "graphics/opengl/GLRenderTarget.h"
#include "graphics/opengl/GLTexture.h"
#include "graphics/opengl/GLRenderingInterface.h"
#include "graphics/RenderSystem.h"
#include <vector>

USING_NAMESPACE_NXS;

GLRenderTarget::GLRenderTarget(uint32_t width, uint32_t height, PixelFormat format)
    : m_width(width), m_height(height), m_format(format)
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
        // Create textures and FBO
        m_colorTexture.reset(renderInterface->CreateTexture());
        TextureDescription desc;
        desc.width = INT_CAST(m_width);
        desc.height = INT_CAST(m_height);
        desc.format = m_format;
        // Begin and alloc empty data:
        m_colorTexture->Begin(desc);
        // Allocate empty texture
        const size_t bytes = desc.GetBufferSize();
        std::vector<uint8_t> empty(bytes, 0);
        m_colorTexture->LoadData(empty.data(), INT_CAST(bytes));
        m_colorTexture->End();

        m_depthTexture.reset(renderInterface->CreateTexture());
        TextureDescription dd;
        dd.width = INT_CAST(m_width);
        dd.height = INT_CAST(m_height);
        dd.format = PixelFormat::Depth;
        m_depthTexture->Begin(dd);
        const size_t db = dd.GetBufferSize();
        std::vector<uint8_t> emptyDepth(db, 0);
        m_depthTexture->LoadData(emptyDepth.data(), INT_CAST(db));
        m_depthTexture->End();

        CALL_GL_FUNC(glGenFramebuffers(1, &m_fbo));
        CALL_GL_FUNC(glBindFramebuffer(GL_FRAMEBUFFER, m_fbo));
        CALL_GL_FUNC(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorTexture->GetHandle(), 0));
        CALL_GL_FUNC(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture->GetHandle(), 0));
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
