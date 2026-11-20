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
    default:
        // No implementation yet.
        NXS_ASSERT(false);
        break;
    }
    return 0;
}

TextureProxy::TextureProxy(const TextureDescription& desc)
    : m_desc(desc)
{
    
}
