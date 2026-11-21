#include "nexus/GLCubeTexture.h"
#include "nexus/NxsGL.h"
#include "nexus/GLTexture.h"

#include "Remotery.h"

USING_NAMESPACE_NXS;

DEFINE_LOG(CubeTexture);

GLCubeTexture::GLCubeTexture(int32_t width, int32_t height)
    : CubeTexture(width, height)
{

}

GLCubeTexture::~GLCubeTexture()
{
    if (m_textureID == 0) return;
    if (IsBinding()) Unbind();

    CALL_GL_FUNC(glDeleteTextures(1, &m_textureID));
    m_textureID = 0;
}

uint32_t GLCubeTexture::Alloc()
{
    CALL_GL_FUNC(glGenTextures(1, &m_textureID));
    return m_textureID;
}

void GLCubeTexture::Release()
{
    if (m_textureID == 0) return;

    CALL_GL_FUNC(glDeleteTextures(1, &m_textureID));
    m_textureID = 0;
}

void GLCubeTexture::Bind() const
{
    rmt_ScopedCPUSample(GLCubeTexture_Bind, 0);
    if (IsBinding()) return;

    CALL_GL_FUNC(glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID));
}

void GLCubeTexture::Unbind() const
{
    rmt_ScopedCPUSample(GLCubeTexture_Unbind, 0);
    CALL_GL_FUNC(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
}

bool GLCubeTexture::IsBinding() const
{
    return false;
}

// void GLCubeTexture::LoadData(Face face, Ref<IBuffer> data)
// {
//     const int index = INT_CAST(face);
//     const int mips = 0;
//     const int border = 0;
//     const GLint gl_pixelFormats = GL::NxsPixelFormatToGL(m_desc.format);
//     const GLint gl_internalFormat = m_desc.colorSpace == TextureColorSpace::SRgb ?
//         GL::GetSRGBFormat(m_desc.format) :
//         gl_pixelFormats;
//     const auto gl_dataType = GL::NxsDataToGLenum(m_desc.componentType);
//     glTexImage2D(
//         GL_TEXTURE_CUBE_MAP_POSITIVE_X + index,
//         mips,
//         gl_internalFormat,
//         m_desc.width, m_desc.height,
//         border,
//         gl_pixelFormats,
//         gl_dataType,
//         data->Data()
//     );
// }

// void GLCubeTexture::LoadData(Array<Ref<IBuffer>> data)
// {
//     if (data.size() < UINT_CAST(Face::Num))
//     {
//         LOG_WARNING(LogCubeTexture, std::format("Invalude array size: {}", data.size()));
//         return;
//     }

//     for (int i = 0; i < INT_CAST(Face::Num); ++i)
//     {
//         LoadData(CAST<Face>(i), data[i]);
//     }
// }

void GLCubeTexture::LoadData(Face face, const uint8_t* data, uint32_t size)
{
    const int index = INT_CAST(face);
    const int mips = 0;
    const int border = 0;
    const GLint gl_pixelFormats = GL::NxsPixelFormatToGL(m_desc.format);
    const GLint gl_internalFormat = m_desc.colorSpace == TextureColorSpace::SRgb ?
        GL::GetSRGBFormat(m_desc.format) :
        gl_pixelFormats;
    const auto gl_dataType = GL::NxsDataToGLenum(m_desc.componentType);
    CALL_GL_FUNC(glTexImage2D(
        GL_TEXTURE_CUBE_MAP_POSITIVE_X + index,
        mips,
        gl_internalFormat,
        m_desc.width, m_desc.height,
        border,
        gl_pixelFormats,
        gl_dataType,
        data
    ));
}