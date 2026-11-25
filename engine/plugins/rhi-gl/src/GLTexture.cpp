//
// Created by nutta on 7/10/2025.
//

#include "nexus/GLTexture.h"

#include "Remotery.h"

USING_NAMESPACE_NXS;

std::atomic<uint32_t> s_bindingTexture(0);

GLTexture::GLTexture(const TextureDescription& desc)
    : TextureProxy(desc)
{

}

void GLTexture::Bind() const
{
    rmt_ScopedCPUSample(GLTexture_Bind, 0);
    if (IsBinding()) return;

    const GLenum target = m_desc.kind == TextureKind::TextureCube
        ? GL_TEXTURE_CUBE_MAP
        : GL_TEXTURE_2D;
    CALL_GL_FUNC(glBindTexture(target, m_textureID));
    s_bindingTexture.store(m_textureID);
}

void GLTexture::Unbind() const
{
    rmt_ScopedCPUSample(GLTexture_Unbind, 0);
    const GLenum target = m_desc.kind == TextureKind::TextureCube
        ? GL_TEXTURE_CUBE_MAP
        : GL_TEXTURE_2D;
    CALL_GL_FUNC(glBindTexture(target, 0));
    if (IsBinding()) s_bindingTexture.store(0);
}

bool GLTexture::IsBinding() const
{
    return m_textureID != 0 && s_bindingTexture.load() == m_textureID;
}

void GLTexture::LoadData(const uint8_t* data, const uint32_t size)
{
    const GLint gl_pixelFormats = GL::NxsPixelFormatToGL(m_desc.format);
    const GLint gl_internalFormat = GL::ResolveTextureInternalFormat(
        m_desc.format,
        m_desc.colorSpace,
        gl_pixelFormats);
    const auto gl_dataType = GL::NxsDataToGLenum(m_desc.componentType);
    if (m_desc.kind == TextureKind::TextureCube)
    {
        for (uint32_t i = 0; i < UINT_CAST(TextureTarget::Num) - 1; ++i)
        {
            CALL_GL_FUNC(glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0,
                gl_internalFormat,
                m_desc.width,
                m_desc.height,
                0,
                gl_pixelFormats,
                gl_dataType,
                data));
        }
        return;
    }
    CALL_GL_FUNC(glTexImage2D(GL_TEXTURE_2D, 0, gl_internalFormat, m_desc.width, m_desc.height, 0, gl_pixelFormats, gl_dataType, data));

    if (m_desc.numMips > 1)
    {
        CALL_GL_FUNC(glGenerateMipmap(GL_TEXTURE_2D));
    }
}

void GLTexture::LoadData(TextureTarget target, const uint8_t* data, uint32_t size)
{
    if (m_desc.kind != TextureKind::TextureCube ||
        target == TextureTarget::Texture2D ||
        UINT_CAST(target) >= UINT_CAST(TextureTarget::Num))
    {
        LoadData(data, size);
        return;
    }

    const GLint gl_pixelFormats = GL::NxsPixelFormatToGL(m_desc.format);
    const GLint gl_internalFormat = GL::ResolveTextureInternalFormat(
        m_desc.format,
        m_desc.colorSpace,
        gl_pixelFormats);
    const auto gl_dataType = GL::NxsDataToGLenum(m_desc.componentType);
    const uint32_t faceIndex = UINT_CAST(target) - UINT_CAST(TextureTarget::Texture2D) - 1;
    CALL_GL_FUNC(glTexImage2D(
        GL_TEXTURE_CUBE_MAP_POSITIVE_X + faceIndex,
        0,
        gl_internalFormat,
        m_desc.width,
        m_desc.height,
        0,
        gl_pixelFormats,
        gl_dataType,
        data));
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
