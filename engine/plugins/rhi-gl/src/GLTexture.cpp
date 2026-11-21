//
// Created by nutta on 7/10/2025.
//

#include "nexus/GLTexture.h"

#include "Remotery.h"

USING_NAMESPACE_NXS;

std::atomic<uint32_t> GLTexture::s_bindingTexture(0);

GLTexture::GLTexture(const TextureDescription& desc)
    : TextureProxy(desc)
{

}

void GLTexture::Bind() const
{
    rmt_ScopedCPUSample(GLTexture_Bind, 0);
    if (IsBinding()) return;

    CALL_GL_FUNC(glBindTexture(GL_TEXTURE_2D, m_textureID));
    s_bindingTexture.store(m_textureID);
}

void GLTexture::Unbind() const
{
    rmt_ScopedCPUSample(GLTexture_Unbind, 0);
    CALL_GL_FUNC(glBindTexture(GL_TEXTURE_2D, 0));
    if (IsBinding()) s_bindingTexture.store(0);
}

bool GLTexture::IsBinding() const
{
    return m_textureID != 0 && s_bindingTexture.load() == m_textureID;
}

void GLTexture::LoadData(const uint8_t* data, const uint32_t size)
{
    const GLint gl_pixelFormats = GL::NxsPixelFormatToGL(m_desc.format);
    const GLint gl_internalFormat = m_desc.colorSpace == TextureColorSpace::SRgb && GL::IsSRGBCapableFormat(m_desc.format) ?
        GL::GetSRGBFormat(m_desc.format) :
        gl_pixelFormats;
    const auto gl_dataType = GL::NxsDataToGLenum(m_desc.componentType);
    CALL_GL_FUNC(glTexImage2D(GL_TEXTURE_2D, 0, gl_internalFormat, m_desc.width, m_desc.height, 0, gl_pixelFormats, gl_dataType, data));

    if (m_desc.numMips > 1)
    {
        CALL_GL_FUNC(glGenerateMipmap(GL_TEXTURE_2D));
    }
}

void GLTexture::LoadMipData(const uint8_t* data, uint32_t size, uint32_t mip)
{
    // TODO:
}

void GLTexture::CopyData(const void* data, size_t bytes, const size_t offset)
{
    NXS_ASSERT_MSG(IsBinding(), std::format("Invalid operation. The texture is unbound."));
    const GLint gl_pixelFormats = GL::NxsPixelFormatToGL(m_desc.format);
    const GLint xOffset = offset % m_desc.width;
    const GLint yOffset = offset / m_desc.width;
    const auto gl_dataType = GL::NxsDataToGLenum(m_desc.componentType);
    CALL_GL_FUNC(glTexSubImage2D(GL_TEXTURE_2D, 0, xOffset, yOffset, m_desc.width, m_desc.height, gl_pixelFormats, gl_dataType, data));
}

uint32_t GLTexture::Alloc()
{
    CALL_GL_FUNC(glGenTextures(1, &m_textureID));
    return m_textureID;
}

void GLTexture::Release()
{
    if (m_textureID == 0) return;

    CALL_GL_FUNC(glDeleteTextures(1, &m_textureID));
    m_textureID = 0;
}

GLTexture::~GLTexture()
{
    if (m_textureID == 0) return;
    if (IsBinding()) Unbind();

    CALL_GL_FUNC(glDeleteTextures(1, &m_textureID));
    m_textureID = 0;
}
