//
// Created by nutta on 7/10/2025.
//

#include <nexus/graphics/opengl/GLTexture.h>

NXS_NAMESPACE
{
    namespace GL
    {
        static GLuint NxsTextureWrapModeToGL(const TextureWrapMode mode)
        {
            switch (mode)
            {
            case TextureWrapMode::Clamp:
                return GL_CLAMP;
            case TextureWrapMode::Repeat:
                return GL_REPEAT;
            case TextureWrapMode::MirroredRepeat:
                return GL_MIRRORED_REPEAT;
            case TextureWrapMode::ClampToEdge:
                return GL_CLAMP_TO_EDGE;
            case TextureWrapMode::ClampToBorder:
                return GL_CLAMP_TO_BORDER;
            case TextureWrapMode::MirroredClampToEdge:
                return GL_MIRROR_CLAMP_TO_EDGE;
            default:
                assert(false);
                return GL_NONE;
            }
        }
    }
}

USING_NAMESPACE_NXS;

static GLuint NxsTextureFilterModeToGL(const TextureFilterMode mode)
{
    switch (mode)
    {
    case TextureFilterMode::Nearest:
        return GL_NEAREST;
    case TextureFilterMode::Linear:
        return GL_LINEAR;
    case TextureFilterMode::NearestMipmapNearest:
        return GL_NEAREST_MIPMAP_NEAREST;
    case TextureFilterMode::LinearMipmapNearest:
        return GL_LINEAR_MIPMAP_NEAREST;
    case TextureFilterMode::NearestMipmapLinear:
        return GL_NEAREST_MIPMAP_LINEAR;
    case TextureFilterMode::LinearMipmapLinear:
        return GL_LINEAR_MIPMAP_LINEAR;
    default:
        assert(false);
        return GL_NONE;
    }
}

void GLTexture::Bind() const
{
    CALL_GL_FUNC(glBindTexture(GL_TEXTURE_2D, m_handle));
}

void GLTexture::Unbind() const
{
    CALL_GL_FUNC(glBindTexture(GL_TEXTURE_2D, 0));
}

TextureProxy& GLTexture::Begin(const TextureCreationInfo& info)
{
    TextureProxy::Begin(info);

    GLuint gl_wrapModeS = GL::NxsTextureWrapModeToGL(info.wrapModeS);
    GLuint gl_wrapModeT = GL::NxsTextureWrapModeToGL(info.wrapModeT);

    GLuint gl_filterMin = NxsTextureFilterModeToGL(info.filterMin);
    GLuint gl_filterMag = NxsTextureFilterModeToGL(info.filterMag);

    CALL_GL_FUNC(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, gl_wrapModeS));
    CALL_GL_FUNC(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, gl_wrapModeT));
    CALL_GL_FUNC(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_filterMin));
    CALL_GL_FUNC(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gl_filterMag));
    return *this;
}

TextureProxy& GLTexture::LoadData(const uint8* data, const uint32 size)
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
    CALL_GL_FUNC(glTexImage2D(GL_TEXTURE_2D, 0, gl_pixelFormats, m_width, m_height, 0, gl_pixelFormats, gl_dataType, data));

    if (m_numMips > 1)
    {
        CALL_GL_FUNC(glGenerateMipmap(GL_TEXTURE_2D));
    }
    return *this;
}

TextureProxy& GLTexture::LoadMipData(const uint8* data, uint32 size, uint32 mip)
{
    return TextureProxy::LoadMipData(data, size, mip);
}

uint32 GLTexture::Alloc()
{
    CALL_GL_FUNC(glGenTextures(1, &m_handle));
    return m_handle;
}

void GLTexture::Release()
{
    CALL_GL_FUNC(glDeleteTextures(1, &m_handle));
}

GLTexture::~GLTexture()
{
    CALL_GL_FUNC(glDeleteTextures(1, &m_handle));
}
