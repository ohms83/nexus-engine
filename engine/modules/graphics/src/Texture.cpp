//
// Created by nutta on 7/9/2025.
//

#include "nexus/graphics/Texture.h"
#include "nexus/graphics/GpuResourceFactory.h"

USING_NAMESPACE_NXS;

DEFINE_LOG(Texture);

Texture::~Texture()
{
    LOG_DEBUG(LogResource, std::format("Unloaded texture: '{}'.", m_path));
}

void Texture::SetSize(int32_t width, int32_t height)
{
    m_desc.width = width;
    m_desc.height = height;
}

void Texture::SetNumChannels(int32_t channels)
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

void Texture::SetNumMips(const int32_t numMips)
{
    m_desc.numMips = numMips;
}

void Texture::DescribeTexture(const TextureDescription& desc)
{
    m_desc = desc;
}

Ref<TextureProxy> Texture::AllocateGpuResource(const uint8_t* pixels)
{
    return AllocateGpuResource(pixels, *GetGpuResourceFactory());
}

Ref<TextureProxy> Texture::AllocateGpuResource(
    const uint8_t* pixels,
    const IGpuResourceFactory& gpuResourceFactory)
{
    if (m_textureProxy != nullptr)
    {
        LOG_ERROR(LogTexture, "TextureProxy was already allocated. If you want to re-allocate, the existing texture must be released first.");
        return m_textureProxy;
    }

    m_textureProxy = gpuResourceFactory.CreateTextureProxy();
    m_textureProxy->Begin(m_desc)
        .LoadData(pixels, m_desc.width * m_desc.height * m_desc.channels)
    .End();
    return m_textureProxy;
}
