#include "nexus/graphics/CubeTexture.h"

USING_NAMESPACE_NXS;

CubeTexture::CubeTexture(int32_t width, int32_t height)
    : TextureProxy( {
        .width = width,
        .height = height,
        .channels = 3,
        .format = PixelFormat::RGB,
        .componentType = DataType::UByte,
        .colorSpace = TextureColorSpace::SRgb,
        .wrapModeS = TextureWrapMode::ClampToEdge,
        .wrapModeT = TextureWrapMode::ClampToEdge,
        .wrapModeR = TextureWrapMode::ClampToEdge,
        .filterMin = TextureFilterMode::Linear,
        .filterMag = TextureFilterMode::Linear,
        .numMips = 1,
    }
)
{

}