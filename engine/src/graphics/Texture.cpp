//
// Created by nutta on 7/9/2025.
//

#include "graphics/Texture.h"
#include "graphics/RenderingInterface.h"

USING_NAMESPACE_NXS;

DEFINE_LOG(Texture);

Texture::~Texture() = default;

void Texture::SetSize(int32 width, int32 height)
{
    m_desc.width = width;
    m_desc.height = height;
}

void Texture::SetNumChannels(int32 channels)
{
    m_desc.channels = channels;
}

void Texture::SetWrapMode(const TextureWrapMode wrapS, const TextureWrapMode wrapT)
{
    m_desc.wrapModeS = wrapS;
    m_desc.wrapModeT = wrapT;
}

void Texture::SetFiltering(const TextureFilterMode minFilter, const TextureFilterMode magFilter)
{
    m_desc.filterMin = minFilter;
    m_desc.filterMag = magFilter;
}

void Texture::SetNumMips(const int32 numMips)
{
    m_desc.numMips = numMips;
}

void Texture::DescribeTexture(const TextureDescription& desc)
{
    m_desc = desc;
}

Ref<TextureProxy> Texture::AllocateGpuResource(const uint8* pixels, const size_t size, Ref<RenderingInterface> renderingInterface)
{
    if (m_textureProxy != nullptr)
    {
        LOG_ERROR(LogTexture, "TextureProxy was already allocated. If you want to re-allocate, the existing texture must be released first.");
        return m_textureProxy;
    }

    m_textureProxy.reset(renderingInterface->CreateTexture());
    m_textureProxy->Begin(m_desc)
        .LoadData(pixels, size)
    .End();
    return m_textureProxy;
}
