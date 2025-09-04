#include "graphics/TextureLoader.h"
#include "memory/OwningBuffer.h"

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
    Ref<TextureBuffer> buffer = PTR_CAST<TextureBuffer>(PerformLoadFile(path));

    const auto texture = std::make_shared<Texture>(path, id);
    texture->DescribeTexture(buffer->desc);
    texture->AllocateGpuResource(buffer->pixels.get(), buffer->desc.GetBufferSize(), m_renderingInterface);
    return PTR_CAST<Resource>(texture);
}

Ref<IBuffer> TextureLoader::PerformLoadFile(const std::string& path)
{
    TextureDescription desc{};
    stbi_set_flip_vertically_on_load(0);
    auto pixels = stbi_load(path.c_str(), &desc.width, &desc.height, &desc.channels, 0);
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

    auto buffer = std::make_shared<TextureBuffer>();
    buffer->pixels.reset(pixels);
    buffer->desc = desc;
    buffer->size = desc.GetBufferSize();
    return buffer;
}