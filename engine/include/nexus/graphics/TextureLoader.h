#pragma once

#include "nexus/core/ResourceLoader.h"
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
        explicit TextureLoader(Ref<RenderingInterface> renderingInterface);

        Ref<Resource> Load(const std::string& path, uint32 id) override;
        void LoadAsync(const std::string& path, uint32 id, const LoaderCallback& onFinishCallback) override;

    private:
        static Ptr<IBuffer> ProcessLoadFile(const std::string& path, TextureDescription& outDesc);
        Ref<RenderingInterface> m_renderingInterface;
    };
}