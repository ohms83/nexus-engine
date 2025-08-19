//
// Created by nutta on 7/9/2025.
//

#include "graphics/Texture.h"
#include "graphics/RenderingInterface.h"

#include <iostream>
#include <ostream>
#include <stb_image.h>

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

TextureLoader::TextureLoader(Ref<RenderingInterface> renderingInterface)
    : m_renderingInterface(renderingInterface)
{
}

Ref<Resource> TextureLoader::Load(const std::string& path, uint32 id)
{
    TextureDescription desc{};
    const auto texture = std::make_shared<Texture>(path, id);
    stbi_set_flip_vertically_on_load(0);
    Ptr<stbi_uc> pixels;
    pixels.reset(stbi_load(path.c_str(), &desc.width, &desc.height, &desc.channels, 0));
    if (!pixels) return nullptr;

    // TODO: Supports more pixel format (SRGB and other compressed textures).
    switch (desc.channels)
    {
    case 1:
        desc.format = PixelFormat::Grey;
        break;
    case 3:
        desc.format = PixelFormat::RGB;
        break;
    case 4:
        desc.format = PixelFormat::RGBA;
        break;
    default:
        NXS_ASSERT_MSG(false, std::format("Texture loading failed! Unsupported pixel format. Texture={} NumChannel={}", path, desc.channels));
        break;
    }
    // TODO: Supporting other component types.
    desc.componentType = DataType::UByte;

    texture->DescribeTexture(desc);
    texture->AllocateGpuResource(pixels.get(), desc.GetBufferSize(), m_renderingInterface);
    return PTR_CAST<Resource>(texture);
}

TextureManager::TextureManager(Ref<RenderingInterface> renderingInterface)
{
    RegisterLoader(std::make_unique<TextureLoader>(renderingInterface));
}