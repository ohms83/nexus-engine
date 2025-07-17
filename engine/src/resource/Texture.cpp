//
// Created by nutta on 7/9/2025.
//

#include <nexus/resource/Texture.h>
// TODO: Might consider replacing this with a more robust loader.
#include <iostream>
#include <ostream>
#include <stb_image.h>

USING_NAMESPACE_NXS;

Texture::~Texture()
{
}

void Texture::SetWrapMode(const TextureWrapMode wrapS, const TextureWrapMode wrapT)
{
    m_wrapModeS = wrapS;
    m_wrapModeT = wrapT;
}

void Texture::SetFiltering(const TextureFilterMode minFilter, const TextureFilterMode magFilter)
{
    m_filterMin = minFilter;
    m_filterMag = magFilter;
}

void Texture::SetNumMips(const int32 numMips)
{
    m_numMips = numMips;
}

TextureProxy* Texture::AllocateGpuResource(const RenderingInterface& renderingInterface, const bool keepCopy)
{
    m_textureProxy.reset(renderingInterface.CreateTexture());
    const TextureCreationInfo info = {
        m_width, m_height, m_channels,
        m_format, m_componentType,
        m_wrapModeS, m_wrapModeT,
        m_filterMin, m_filterMag,
        m_numMips,
    };
    m_textureProxy->Begin(info)
        .LoadData(m_data.Data(), m_data.Size())
    .End();

    if (!keepCopy) m_data.Release();
    return m_textureProxy.get();
}

uint8* Texture::Load_Impl(const std::string& path, size_t& out_size)
{
    stbi_set_flip_vertically_on_load(true);
    const auto pixels = stbi_load(path.c_str(), &m_width, &m_height, &m_channels, 0);
    if (!pixels)
    {
        std::cout << "Failed to load " << path << std::endl;
        return nullptr;
    }

    out_size = m_width * m_height * m_channels;

    // TODO: Supports more pixel format (SRGB and other compressed textures).
    switch (m_channels)
    {
    case 3:
        m_format = PixelFormat::RGB;
        break;
    case 4:
        m_format = PixelFormat::RGBA;
        break;
    default:
        // TODO: Handle gray scale images.
        assert(false);
        break;
    }
    // TODO: Supporting other component types.
    m_componentType = DataType::UByte;
    return pixels;
}
