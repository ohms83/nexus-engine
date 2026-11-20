#include "nexus/graphics/CubeTexture.h"

USING_NAMESPACE_NXS;

CubeTexture::CubeTexture(int32_t width, int32_t height)
{
    m_desc.width = width;
    m_desc.height = height;
    m_desc.format = PixelFormat::RGB;
    m_desc.channels = 3;
    m_desc.colorSpace = TextureColorSpace::Linear;
    m_desc.componentType = DataType::UByte;
    m_desc.numMips = 1;
    m_desc.filterMag = TextureFilterMode::Linear;
    m_desc.filterMin = TextureFilterMode::Linear;
    m_desc.wrapModeS = TextureWrapMode::ClampToEdge;
    m_desc.wrapModeT = TextureWrapMode::ClampToEdge;
    m_desc.wrapModeR = TextureWrapMode::ClampToEdge;
}