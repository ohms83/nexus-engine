#include "graphics/TextureLoader.h"
#include "memory/OwningBuffer.h"
#include "nexus/task/FutureWaitingTask.h"

#include <future>
#include <ostream>

#include <stb_image.h>

USING_NAMESPACE_NXS;

namespace
{
    struct TextureLoadedData
    {
        Ptr<stbi_uc> pixels;
        TextureDescription desc;
    };

    TextureLoadedData ProcessLoadFile(const std::string& path)
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
        case 2:
            desc.format = PixelFormat::RedGreen;
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
    }
}

TextureLoader::TextureLoader(const Ref<RenderingInterface>& renderingInterface)
    : m_renderingInterface(renderingInterface)
{
}

Ref<Resource> TextureLoader::Load(const std::string& path, uint32_t id)
{
    auto [pixels, desc] = ProcessLoadFile(path);

    const auto texture = std::make_shared<Texture>(path, id);
    texture->DescribeTexture(desc);
    texture->AllocateGpuResource(pixels.get(), desc.GetBufferSize(), m_renderingInterface);
    return PTR_CAST<Resource>(texture);
}

Ref<IResourceLoader::LoadResult> TextureLoader::LoadAsync(const std::string& path, uint32_t id, TaskScheduler& scheduler, Callback onFinishCallback)
{
    const auto result = std::make_shared<LoadResult>();
    result->path = path;
    result->status.store(LoadResult::Status::Loading);

    std::future<TextureLoadedData> future = std::async(std::launch::async, [path]
    {
        return ProcessLoadFile(path);
    });

    const auto waitingTask = std::make_shared<FutureWaitingTask<TextureLoadedData>>(std::move(future), [onFinishCallback, path, id, result, renderInterface = m_renderingInterface](const TextureLoadedData& loadedData)
    {
        if (!loadedData.pixels)
        {
            result->status.store(LoadResult::Status::Failed);
            onFinishCallback(nullptr);
            return;
        }

        const auto texture = std::make_shared<Texture>(path, id);
        texture->DescribeTexture(loadedData.desc);
        texture->AllocateGpuResource(loadedData.pixels.get(), loadedData.desc.GetBufferSize(), renderInterface);

        result->status.store(LoadResult::Status::Ready);
        result->resource = texture;
        onFinishCallback(texture);
    });

    scheduler.ScheduleTask(waitingTask);
    return result;
}
