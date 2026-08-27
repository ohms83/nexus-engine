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
                NXS_ASSERT(false);
            }
            return GL_NONE;
        }

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
                NXS_ASSERT(false);
            }
            return GL_NONE;
        }

        static GLint NxsPixelFormatToGL(const PixelFormat format)
        {
            switch (format)
            {
            case PixelFormat::None:
                break;
            case PixelFormat::Grey:
            case PixelFormat::Red:
                // OpenGL has no explicit Greyscale format. The greyscale textures are normally
                // represented by a single channel red component.
                return GL_RED;
            case PixelFormat::Green:
                return GL_GREEN;
            case PixelFormat::Blue:
                return GL_BLUE;
            case PixelFormat::Alpha:
                return GL_RGBA;
            case PixelFormat::RedGreen:
                return GL_RG;
            case PixelFormat::RGB:
                return GL_RGB;
            case PixelFormat::RGBA:
                return GL_RGBA;
            case PixelFormat::Depth:
                return GL_DEPTH_COMPONENT;
            case PixelFormat::Stencil:
                return GL_STENCIL_INDEX;
            default:
                NXS_ASSERT(false);
            }
            return GL_NONE;
        }
    }
}

USING_NAMESPACE_NXS;

std::atomic<uint32_t> GLTexture::s_bindingTexture(0);

void GLTexture::Bind() const
{
    CALL_GL_FUNC(glBindTexture(GL_TEXTURE_2D, m_textureID));
    s_bindingTexture.store(m_textureID);
}

void GLTexture::Unbind() const
{
    CALL_GL_FUNC(glBindTexture(GL_TEXTURE_2D, 0));
    if (IsBinding()) s_bindingTexture.store(0);
}

bool GLTexture::IsBinding() const
{
    return m_textureID != 0 && s_bindingTexture.load() == m_textureID;
}

TextureProxy& GLTexture::Begin(const TextureDescription& info)
{
    TextureProxy::Begin(info);

    GLuint gl_wrapModeS = GL::NxsTextureWrapModeToGL(info.wrapModeS);
    GLuint gl_wrapModeT = GL::NxsTextureWrapModeToGL(info.wrapModeT);

    GLuint gl_filterMin = GL::NxsTextureFilterModeToGL(info.filterMin);
    GLuint gl_filterMag = GL::NxsTextureFilterModeToGL(info.filterMag);

    CALL_GL_FUNC(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, gl_wrapModeS));
    CALL_GL_FUNC(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, gl_wrapModeT));
    CALL_GL_FUNC(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_filterMin));
    CALL_GL_FUNC(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gl_filterMag));
    return *this;
}

TextureProxy& GLTexture::LoadData(const uint8_t* data, const uint32_t size)
{
    TextureProxy::LoadData(data, size);

    GLint gl_pixelFormats = GL::NxsPixelFormatToGL(m_format);
    const auto gl_dataType = GL::NxsDataToGLenum(m_componentType);
    CALL_GL_FUNC(glTexImage2D(GL_TEXTURE_2D, 0, gl_pixelFormats, m_width, m_height, 0, gl_pixelFormats, gl_dataType, data));

    if (m_numMips > 1)
    {
        CALL_GL_FUNC(glGenerateMipmap(GL_TEXTURE_2D));
    }
    return *this;
}

TextureProxy& GLTexture::LoadMipData(const uint8_t* data, uint32_t size, uint32_t mip)
{
    return TextureProxy::LoadMipData(data, size, mip);
}

void GLTexture::CopyData(const void* data, size_t bytes, const size_t offset)
{
    NXS_ASSERT_MSG(IsBinding(), std::format("Invalid operation. The texture is unbound."));
    const GLint gl_pixelFormats = GL::NxsPixelFormatToGL(m_format);
    const GLint xOffset = offset % m_width;
    const GLint yOffset = offset / m_width;
    const auto gl_dataType = GL::NxsDataToGLenum(m_componentType);
    CALL_GL_FUNC(glTexSubImage2D(GL_TEXTURE_2D, 0, xOffset, yOffset, m_width, m_height, gl_pixelFormats, gl_dataType, data));
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
