//
// Created by nutta on 7/10/2025.
//

#include "nexus/GLTexture.h"
#include "nexus/math/Math.h"

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
    rmt_ScopedCPUSample(GLTexture_LoadData, 0);
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
    rmt_ScopedCPUSample(GLTexture_LoadData, 0);
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
    rmt_ScopedCPUSample(GLTexture_LoadMipData, 0);
    NXS_ASSERT_MSG(data, "Mip data must not be null.");
    NXS_ASSERT_MSG(m_desc.numMips == 0 || mip < m_desc.numMips,
        std::format("Mip level {} is outside the texture's {} mip levels.", mip, m_desc.numMips));
    NXS_ASSERT_MSG(IsBinding(), "Invalid operation. The texture is unbound.");

    const GLint mipWidth = std::max(1, m_desc.width >> mip);
    const GLint mipHeight = std::max(1, m_desc.height >> mip);
    const GLint gl_pixelFormats = GL::NxsPixelFormatToGL(m_desc.format);
    const GLint gl_internalFormat = GL::ResolveTextureInternalFormat(
        m_desc.format,
        m_desc.colorSpace,
        gl_pixelFormats);
    const auto gl_dataType = GL::NxsDataToGLenum(m_desc.componentType);

    const size_t bytesPerPixel = m_desc.channels * NxsDataTypeSize(m_desc.componentType);
    NXS_ASSERT_MSG(size >= static_cast<uint32_t>(mipWidth) * static_cast<uint32_t>(mipHeight) * bytesPerPixel,
        "Mip data size is smaller than the mip level dimensions.");

    if (m_desc.kind == TextureKind::TextureCube)
    {
        for (uint32_t i = 0; i < UINT_CAST(TextureTarget::Num) - 1; ++i)
        {
            CALL_GL_FUNC(glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                CAST<GLint>(mip),
                gl_internalFormat,
                mipWidth,
                mipHeight,
                0,
                gl_pixelFormats,
                gl_dataType,
                data));
        }
        return;
    }

    CALL_GL_FUNC(glTexImage2D(
        GL_TEXTURE_2D,
        CAST<GLint>(mip),
        gl_internalFormat,
        mipWidth,
        mipHeight,
        0,
        gl_pixelFormats,
        gl_dataType,
        data));
}

void GLTexture::GenerateMipmaps()
{
    rmt_ScopedCPUSample(GLTexture_GenerateMipmaps, 0);
    NXS_ASSERT_MSG(IsBinding(), "Invalid operation. The texture is unbound.");
    if (m_desc.kind != TextureKind::Texture2D) return;
    CALL_GL_FUNC(glGenerateMipmap(GL_TEXTURE_2D));

    // Update the number of mipmap to reflect the real value.
    m_desc.numMips = Math::ComputeMaxMipLevel(m_desc.width, m_desc.height);
}

bool GLTexture::ReadTexel(const uint32_t mip, void* data, const size_t size)
{
    rmt_ScopedCPUSample(GLTexture_ReadTexel, 0);
    NXS_ASSERT_MSG(IsBinding(), "Invalid operation. The texture is unbound.");
    if (!data ||
        size < 16 ||
        m_desc.kind != TextureKind::Texture2D ||
        mip >= m_desc.numMips)
    {
        return false;
    }

    const int32_t mipWidth = std::max(1, m_desc.width >> mip);
    const int32_t mipHeight = std::max(1, m_desc.height >> mip);
    if (mipWidth != 1 || mipHeight != 1) return false;

    const GLint glPixelFormat = GL::NxsPixelFormatToGL(m_desc.format);
    const GLenum glDataType = GL::NxsDataToGLenum(m_desc.componentType);
    CALL_GL_FUNC(glGetTexImage(GL_TEXTURE_2D, CAST<GLint>(mip), glPixelFormat, glDataType, data));
    return true;
}

bool GLTexture::ReadTexels(uint32_t mip, Ref<IBuffer> outBuffer)
{
    rmt_ScopedCPUSample(GLTexture_ReadTexels, 0);
    NXS_ASSERT_MSG(IsBinding(), "Invalid operation. The texture is unbound.");
    if (m_desc.kind != TextureKind::Texture2D) return false;

    NXS_ASSERT(outBuffer->Size() >= m_desc.GetBufferSize());
    CALL_GL_FUNC(glGetTexImage(GL_TEXTURE_2D, CAST<GLint>(mip), GL_RGBA, GL_FLOAT, outBuffer->Data()));
    return true;
}

void GLTexture::CopyData(const void* data, size_t bytes, const size_t offset)
{
    rmt_ScopedCPUSample(GLTexture_CopyData, 0);
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
