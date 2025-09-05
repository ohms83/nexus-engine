#include "graphics/TextureLoader.h"
#include "memory/OwningBuffer.h"
#include "core/task/FutureWaitingTask.h"

#include <future>
#include <ostream>

#include <stb_image.h>

USING_NAMESPACE_NXS;

TextureLoader::TextureLoader(const Ref<RenderingInterface>& renderingInterface)
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

void TextureLoader::LoadAsync(const std::string& path, uint32 id, TaskScheduler& scheduler, Callback onFinishCallback)
{
    struct TextureLoadedData
    {
        Ptr<stbi_uc> pixels;
        TextureDescription desc;
    };

    std::future<TextureLoadedData> future = std::async(std::launch::async, [path]
    {
        TextureLoadedData result {};
        TextureDescription& desc = result.desc;

        stbi_set_flip_vertically_on_load(0);
        const auto pixels = stbi_load(path.c_str(), &desc.width, &desc.height, &desc.channels, 0);
        if (!pixels) return TextureLoadedData {};

        result.pixels.reset(pixels);

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
        return result;
    });

    const auto waitingTask = std::make_shared<FutureWaitingTask<TextureLoadedData>>(std::move(future), [onFinishCallback, path, id, renderInterface = m_renderingInterface](const TextureLoadedData& loadedData)
    {
        const auto texture = std::make_shared<Texture>(path, id);
        texture->DescribeTexture(loadedData.desc);
        texture->AllocateGpuResource(loadedData.pixels.get(), loadedData.desc.GetBufferSize(), renderInterface);
        onFinishCallback(texture);
    });

    scheduler.ScheduleTask(waitingTask);
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
