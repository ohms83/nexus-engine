#include "graphics/TextureLoader.h"

#include <iostream>
#include <ostream>
#include <stb_image.h>

#include "core/TaskManager.h"
#include "memory/OwningBuffer.h"

USING_NAMESPACE_NXS;

static OwningBuffer s_sharedBuffer;
static std::mutex s_sharedMutex;

TextureLoader::TextureLoader(Ref<RenderingInterface> renderingInterface)
    : m_renderingInterface(renderingInterface)
{
}

Ref<Resource> TextureLoader::Load(const std::string& path, uint32 id)
{
    TextureDescription desc{};
    const auto texture = std::make_shared<Texture>(path, id);
    const auto pixels = ProcessLoadFile(path, desc);
    if (!pixels) return nullptr;

    texture->DescribeTexture(desc);
    texture->AllocateGpuResource(pixels->Data(), pixels->Size(), m_renderingInterface);
    return PTR_CAST<Resource>(texture);
}

void TextureLoader::LoadAsync(const std::string& path, uint32 id, const LoaderCallback& onFinishCallback)
{
    const auto texture = std::make_shared<Texture>(path, id);

    auto workerThreadTask = [this, path, onFinishCallback, texture]()
    {
        TextureDescription desc{};
        auto pixels = ProcessLoadFile(path, desc);
        if (!pixels)
        {
            LOG_ERROR(LogResource, std::format("Texture loading failed! Path={}", path));
            return;
        }

        {
            std::lock_guard<std::mutex> lock(s_sharedMutex);
            s_sharedBuffer.CopyData(pixels->Data(), pixels->Size(), 0);
        }

        auto mainThreadTask = [this, onFinishCallback, texture]()
        {
            texture->AllocateGpuResource(s_sharedBuffer.Data(), s_sharedBuffer.Size(), m_renderingInterface);
            onFinishCallback(PTR_CAST<Resource>(texture));
        };

        TaskManager::Instance().CreateTask(mainThreadTask);
    };

    std::jthread workThread(workerThreadTask);
}

Ptr<IBuffer> TextureLoader::ProcessLoadFile(const std::string& path, TextureDescription& outDesc)
{
    stbi_set_flip_vertically_on_load(0);
    auto buffer = std::make_unique<OwningBuffer>();
    stbi_uc* pixels = stbi_load(path.c_str(), &outDesc.width, &outDesc.height, &outDesc.channels, 0);
    if (!pixels) return nullptr;

    // TODO: Supports more pixel format (SRGB and other compressed textures).
    switch (outDesc.channels)
    {
    case 1:
        outDesc.format = PixelFormat::Grey;
        break;
    case 3:
        outDesc.format = PixelFormat::RGB;
        break;
    case 4:
        outDesc.format = PixelFormat::RGBA;
        break;
    default:
        NXS_ASSERT_MSG(false, std::format("Texture loading failed! Unsupported pixel format. Texture={} NumChannel={}",
            path, outDesc.channels));
        break;
    }
    // TODO: Supporting other component types.
    outDesc.componentType = DataType::UByte;
    buffer->Take(pixels, outDesc.GetBufferSize());
    return buffer;
}
