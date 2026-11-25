//
// Created by nutta on 7/10/2025.
//
#include "nexus/graphics/TextureProxy.h"
#include "nexus/debug/LogDispatcher.h"

USING_NAMESPACE_NXS;

DEFINE_LOG(Textureproxy);

size_t TextureDescription::GetBufferSize() const
{
    switch (componentType)
    {
    case DataType::UByte:
        return width * height * channels;
    case DataType::Float:
        return width * height * channels * sizeof(float);
    default:
        // No implementation yet.
        NXS_ASSERT(false);
        break;
    }
    return 0;
}

TextureDescription TextureDescription::DepthTextureDesc(int32_t width, int32_t height)
{
    TextureDescription depthDesc;
    depthDesc.width = width;
    depthDesc.height = height;
    depthDesc.channels = 1;
    depthDesc.format = PixelFormat::Depth;
    depthDesc.componentType = DataType::Float;
    depthDesc.filterMin = TextureFilterMode::Nearest;
    depthDesc.filterMag = TextureFilterMode::Nearest;
    depthDesc.wrapModeS = TextureWrapMode::ClampToEdge;
    depthDesc.wrapModeT = TextureWrapMode::ClampToEdge;
    return depthDesc;
}

TextureProxy::TextureProxy(const TextureDescription& desc)
    : m_desc(desc)
{
    
}
