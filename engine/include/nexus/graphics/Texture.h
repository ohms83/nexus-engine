//
// Created by nutta on 7/9/2025.
//

#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/resource/Resource.h"
#include "nexus/resource/ResourceLoader.h"
#include "nexus/resource/ResourceManager.h"
#include "TextureProxy.h"
#include "RenderingInterface.h"

NXS_NAMESPACE
{
    class Texture final : public Resource
    {
    public:
        explicit Texture(std::string path, const uint32_t resourceId)
            : Resource(std::move(path), resourceId)
        {
        }

        ~Texture() override;

        void SetSize(int32_t width, int32_t height);
        void SetNumChannels(int32_t channels);
        void SetWrapMode(TextureWrapMode wrapS, TextureWrapMode wrapT);
        void SetFiltering(TextureFilterMode minFilter, TextureFilterMode magFilter);
        void SetNumMips(int32_t numMips);
        void DescribeTexture(const TextureDescription& desc);

        MAYBE_UNUSED Ref<TextureProxy> AllocateGpuResource(const uint8_t* pixels, size_t size, Ref<RenderingInterface> renderingInterface);

        NODISCARD Ref<TextureProxy> GetProxy() const
        {
            return m_textureProxy;
        }

    protected:
        TextureDescription m_desc{};
        Ref<TextureProxy> m_textureProxy;
    };
}