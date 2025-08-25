#include "graphics/TextureLoader.h"

#include <iostream>
#include <ostream>
#include <stb_image.h>

USING_NAMESPACE_NXS;

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