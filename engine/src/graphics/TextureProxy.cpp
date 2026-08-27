//
// Created by nutta on 7/10/2025.
//
#include "nexus/graphics/TextureProxy.h"
#include "nexus/core/LogDispatcher.h"

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
        assert(false);
        break;
    }
    return 0;
}

TextureProxy::~TextureProxy() = default;

TextureProxy& TextureProxy::Begin(const TextureDescription& info)
{
    NXS_ASSERT_MSG(m_textureID == 0, std::format("Texture is already created."));
    m_textureID = Alloc();

    m_width = info.width;
    m_height = info.height;
    m_format = info.format;
    m_channels = info.channels;
    m_componentType = info.componentType;
    m_filterMin = info.filterMin;
    m_filterMag = info.filterMag;
    m_wrapModeS = info.wrapModeS;
    m_wrapModeT = info.wrapModeT;
    m_numMips = info.numMips;

    Bind();
    return *this;
}

TextureProxy& TextureProxy::LoadData(const uint8_t* data, uint32_t size)
{
    assert(m_textureID != 0);
    return *this;
}

TextureProxy& TextureProxy::LoadMipData(const uint8_t* data, uint32_t size, uint32_t mip)
{
    assert(m_textureID != 0);
    return *this;
}

void TextureProxy::End()
{
    assert(m_textureID != 0);
    Unbind();
}
