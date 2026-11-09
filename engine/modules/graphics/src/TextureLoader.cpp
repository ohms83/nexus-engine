#include "nexus/graphics/TextureLoader.h"
#include "nexus/memory/OwningBuffer.h"
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

    TextureLoadedData ProcessLoadFile(const std::string& path, const TextureColorSpace colorSpace)
    {
        TextureLoadedData result {};
        TextureDescription& desc = result.desc;

        stbi_set_flip_vertically_on_load(0);
        const auto pixels = stbi_load(path.c_str(), &desc.width, &desc.height, &desc.channels, 0);
        if (!pixels) return TextureLoadedData {};

        result.pixels.reset(pixels);

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
        desc.colorSpace = colorSpace;
        return result;
    }
}

TextureLoader::TextureLoader(const Ref<RenderingInterface>& renderingInterface)
    : m_renderingInterface(renderingInterface)
{
}

Ref<Resource> TextureLoader::Load(const std::string& path, uint32_t id)
{
    return Load(path, id, UINT_CAST(TextureColorSpace::Linear));
}

Ref<Resource> TextureLoader::Load(const std::string& path, uint32_t id, const uint32_t variant)
{
    if (variant > UINT_CAST(TextureColorSpace::SRgb))
    {
        LOG_ERROR(LogResource, std::format("Invalid texture color-space variant {} for '{}'.", variant, path));
        return nullptr;
    }

    const auto colorSpace = CAST<TextureColorSpace>(variant);
    auto [pixels, desc] = ProcessLoadFile(path, colorSpace);
    if (!pixels)
    {
        LOG_ERROR(LogResource, std::format("Failed to load texture '{}'.", path));
        return nullptr;
    }

    const auto texture = std::make_shared<Texture>(path, id);
    texture->DescribeTexture(desc);
    texture->AllocateGpuResource(pixels.get(), desc.GetBufferSize(), m_renderingInterface);
    return PTR_CAST<Resource>(texture);
}

Ref<IResourceLoader::LoadResult> TextureLoader::LoadAsync(const std::string& path, uint32_t id, TaskScheduler& scheduler, Callback onFinishCallback)
{
    return LoadAsync(path, id, scheduler, UINT_CAST(TextureColorSpace::Linear), std::move(onFinishCallback));
}

Ref<IResourceLoader::LoadResult> TextureLoader::LoadAsync(
    const std::string& path,
    uint32_t id,
    TaskScheduler& scheduler,
    const uint32_t variant,
    Callback onFinishCallback)
{
    const auto result = std::make_shared<LoadResult>();
    result->path = path;
    if (variant > UINT_CAST(TextureColorSpace::SRgb))
    {
        result->error = std::format("Invalid texture color-space variant {}.", variant);
        result->status.store(LoadResult::Status::Failed);
        LOG_ERROR(LogResource, std::format("{} Texture='{}'.", result->error, path));
        onFinishCallback(nullptr);
        return result;
    }
    result->status.store(LoadResult::Status::Loading);

    const auto colorSpace = CAST<TextureColorSpace>(variant);
    std::future<TextureLoadedData> future = std::async(std::launch::async, [path, colorSpace]
    {
        return ProcessLoadFile(path, colorSpace);
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
