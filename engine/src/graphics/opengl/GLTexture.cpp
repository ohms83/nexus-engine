//
// Created by nutta on 7/10/2025.
//

#include <nexus/graphics/opengl/GLTexture.h>

USING_NAMESPACE_NXS;

void GLTexture::Bind() const
{
    glBindTexture(GL_TEXTURE_2D, m_handle);
    CHECK_GL_ERROR();
}

void GLTexture::Unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
    CHECK_GL_ERROR();
}

TextureProxy& GLTexture::Begin(const TextureCreationInfo& info)
{
    TextureProxy::Begin(info);

    const std::array<GLuint, SIZE_CAST(TextureWrapMode::Num)> gl_wrapModes = {
        // None
        GL_NONE,
        // Clamp
        GL_CLAMP,
        // Repeat
        GL_REPEAT,
        // MirroredRepeat
        GL_MIRRORED_REPEAT,
        // ClampToEdge
        GL_MIRRORED_REPEAT,
        // ClampToBorder
        GL_CLAMP_TO_BORDER,
        // MirroredClampToEdgex
        GL_MIRROR_CLAMP_TO_EDGE,
    };
    assert(info.wrapModeS != TextureWrapMode::Num);
    assert(info.wrapModeT != TextureWrapMode::Num);

    const std::array<GLuint, SIZE_CAST(TextureFilterMode::Num)> gl_filterModes = {
        // None
        GL_NONE,
        // Nearest
        GL_NEAREST,
        // Linear
        GL_LINEAR,
        // NearestMipmapNearest
        GL_NEAREST_MIPMAP_NEAREST,
        // LinearMipmapLinear
        GL_LINEAR_MIPMAP_LINEAR,
        // NearestMipmapLinear
        GL_NEAREST_MIPMAP_LINEAR,
        // LinearMipmapNearest
        GL_LINEAR_MIPMAP_NEAREST,
    };
    assert(info.filterMag != TextureFilterMode::Num);
    assert(info.filterMin != TextureFilterMode::Num);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, gl_wrapModes[INT_CAST(info.wrapModeS)]);
    CHECK_GL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, gl_wrapModes[INT_CAST(info.wrapModeT)]);
    CHECK_GL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_filterModes[INT_CAST(info.filterMag)]);
    CHECK_GL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gl_filterModes[INT_CAST(info.filterMin)]);
    CHECK_GL_ERROR();
    return *this;
}

TextureProxy& GLTexture::LoadData(const uint8* data, uint32 size)
{
    TextureProxy::LoadData(data, size);

    GLint gl_pixelFormats = 0;
    switch (m_format)
    {
    case PixelFormat::None:
        break;
    case PixelFormat::Red:
        gl_pixelFormats = GL_RED;
        break;
    case PixelFormat::Green:
        gl_pixelFormats = GL_GREEN;
        break;
    case PixelFormat::Blue:
        gl_pixelFormats = GL_BLUE;
        break;
    case PixelFormat::Alpha:
        gl_pixelFormats = GL_RGBA;
        break;
    case PixelFormat::RGB:
        gl_pixelFormats = GL_RGB;
        break;
    case PixelFormat::RGBA:
        gl_pixelFormats = GL_RGBA;
        break;
    case PixelFormat::Depth:
        gl_pixelFormats = GL_DEPTH_COMPONENT;
        break;
    case PixelFormat::Stencil:
        gl_pixelFormats = GL_STENCIL_INDEX;
        break;
    default:
        assert(false);
        break;
    }

    const auto gl_dataType = nexus::GL::NxsDataToGLenum(m_componentType);
    glTexImage2D(GL_TEXTURE_2D, 0, gl_pixelFormats, m_width, m_height, 0, gl_pixelFormats, gl_dataType, data);
    CHECK_GL_ERROR();

    if (m_numMips > 1)
    {
        glGenerateMipmap(GL_TEXTURE_2D);
        CHECK_GL_ERROR();
    }
    return *this;
}

TextureProxy& GLTexture::LoadMipData(const uint8* data, uint32 size, uint32 mip)
{
    return TextureProxy::LoadMipData(data, size, mip);
}

uint32 GLTexture::Alloc()
{
    glGenTextures(1, &m_handle);
    CHECK_GL_ERROR();
    return m_handle;
}

void GLTexture::Release()
{
    glDeleteTextures(1, &m_handle);
    CHECK_GL_ERROR();
}

GLTexture::~GLTexture()
{
    glDeleteTextures(1, &m_handle);
    CHECK_GL_ERROR();
}
