#pragma once

#include "nexus/core/resource/ResourceLoader.h"
#include "RenderingInterface.h"
#include "Texture.h"

NXS_NAMESPACE
{
    /**
     * @brief Implements IResourceLoader to load Texture resources.
     * Encapsulates file reading and GPU texture creation.
     */
    class TextureLoader final : public IResourceLoader
    {
    public:
        explicit TextureLoader(const Ref<RenderingInterface>& renderingInterface);

        MAYBE_UNUSED Ref<Resource> Load(const std::string& path, uint32_t id) override;
        MAYBE_UNUSED Ref<LoadResult> LoadAsync(const std::string& path, uint32_t id, TaskScheduler& scheduler, Callback onFinishCallback) override;

    private:
        Ref<RenderingInterface> m_renderingInterface;
    };
}