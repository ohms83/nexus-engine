
#include "nexus/GLRenderTarget.h"
#include "nexus/GLTexture.h"
#include "nexus/GLRenderingInterface.h"
#include "nexus/graphics/RenderSystem.h"
#include "nexus/container/Array.h"

#include "tracy/Tracy.hpp"

USING_NAMESPACE_NXS;

GLRenderTarget::GLRenderTarget(
    const uint32_t width,
    const uint32_t height,
    Ref<TextureProxy> colorTexture,
    Ref<TextureProxy> depthTexture)
    : m_colorTexture(std::move(colorTexture))
    , m_depthTexture(std::move(depthTexture))
    , m_width(width)
    , m_height(height)
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
    ZoneScopedN("GLRenderTarget_Bind");
    auto renderInterface = rs.GetRenderInterface();
    if (!m_initialized)
    {
        if (m_colorTexture)
        {
            const auto& colorDesc = m_colorTexture->GetDescription();
            const size_t bytes = colorDesc.GetBufferSize();
            Array<uint8_t> empty(bytes, 0);
            m_colorTexture->Bind();
            m_colorTexture->LoadData(empty.data(), INT_CAST(bytes));
            m_colorTexture->Unbind();
        }

        NXS_ASSERT_MSG(m_depthTexture, "Render target requires a depth texture.");
        m_depthTexture->Bind();
        m_depthTexture->LoadData(nullptr, 0);
        m_depthTexture->Unbind();

        CALL_GL_FUNC(glGenFramebuffers(1, &m_fbo));
        CALL_GL_FUNC(glBindFramebuffer(GL_FRAMEBUFFER, m_fbo));
        if (m_colorTexture)
        {
            CALL_GL_FUNC(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorTexture->GetHandle(), 0));
        }
        if (m_depthTexture->GetDescription().kind == TextureKind::TextureCube)
        {
            CALL_GL_FUNC(glFramebufferTexture2D(
                GL_FRAMEBUFFER,
                GL_DEPTH_ATTACHMENT,
                GL_TEXTURE_CUBE_MAP_POSITIVE_X,
                m_depthTexture->GetHandle(),
                0));
        }
        else
        {
            CALL_GL_FUNC(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture->GetHandle(), 0));
        }
        if (!m_colorTexture)
        {
            CALL_GL_FUNC(glDrawBuffer(GL_NONE));
            CALL_GL_FUNC(glReadBuffer(GL_NONE));
        }
        const auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        NXS_ASSERT_MSG(status == GL_FRAMEBUFFER_COMPLETE,
            std::format(
                "Render target framebuffer is incomplete (status: 0x{:X}, size: {}x{}, color: {}).",
                status,
                m_width,
                m_height,
                m_colorTexture != nullptr));
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

void GLRenderTarget::BindCubeFace(RenderSystem& rs, TextureTarget face)
{
    if (m_depthTexture->GetDescription().kind != TextureKind::TextureCube ||
        face == TextureTarget::Texture2D ||
        UINT_CAST(face) >= UINT_CAST(TextureTarget::Num))
    {
        return;
    }

    const uint32_t faceIndex = UINT_CAST(face) - UINT_CAST(TextureTarget::Texture2D) - 1;
    CALL_GL_FUNC(glBindFramebuffer(GL_FRAMEBUFFER, m_fbo));
    CALL_GL_FUNC(glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        GL_TEXTURE_CUBE_MAP_POSITIVE_X + faceIndex,
        m_depthTexture->GetHandle(),
        0));
}
