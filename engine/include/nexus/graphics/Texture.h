//
// Created by nutta on 7/9/2025.
//

#pragma once

#include "nexus/NxsDefine.h"
#include "core/Resource.h"
#include "core/ResourceLoader.h"
#include "core/ResourceManager.h"
#include "TextureProxy.h"

NXS_NAMESPACE
{
    class Texture final : public Resource
    {
    public:
        explicit Texture(std::string path, const uint32 resourceId)
            : Resource(std::move(path), resourceId)
        {
        }

        ~Texture() override;

        void SetSize(int32 width, int32 height);
        void SetNumChannels(int32 channels);
        void SetWrapMode(TextureWrapMode wrapS, TextureWrapMode wrapT);
        void SetFiltering(TextureFilterMode minFilter, TextureFilterMode magFilter);
        void SetNumMips(int32 numMips);
        void DescribeTexture(const TextureDescription& desc);

        MAYBE_UNUSED Ref<TextureProxy> AllocateGpuResource(const uint8* pixels, size_t size);

        NODISCARD Ref<TextureProxy> GetProxy() const
        {
            return m_textureProxy;
        }

    protected:
        TextureDescription m_desc{};
        Ref<TextureProxy> m_textureProxy;
    };

    /**
     * @brief Implements IResourceLoader to load Texture resources.
     * Encapsulates file reading and GPU texture creation.
     */
    class TextureLoader final : public IResourceLoader
    {
    public:
        Ref<Resource> Load(const std::string& path, uint32 id) override;

        std::type_index GetResourceType() const override
        {
            return typeid(Texture);
        }
    };

    class TextureManager : public ResourceManager<Texture>
    {
    public:
        TextureManager();
    };
}