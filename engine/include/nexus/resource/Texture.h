//
// Created by nutta on 7/9/2025.
//

#pragma once

#include <nexus/NxsDefine.h>
#include "Resource.h"
#include "ResourceManager.h"
#include <nexus/graphics/RenderingInterface.h>
#include <nexus/graphics/TextureProxy.h>

NXS_NAMESPACE
{
    class Texture final : public Resource
    {
    public:
        explicit Texture(const uint32 hash)
            : Resource(hash)
        {
        }

        ~Texture() override;

        void SetWrapMode(TextureWrapMode wrapS, TextureWrapMode wrapT);
        void SetFiltering(TextureFilterMode minFilter, TextureFilterMode magFilter);
        void SetNumMips(int32 numMips);

        /**
         * Create a TextureProxy object representing this texture on GPU.
         * @param renderingInterface
         * @param keepCopy Whether to keep the local copy on RAM. The default behavior is to discard
         * the local copy as soon as the GPU resource is allocated.
         * @return
         */
        TextureProxy* AllocateGpuResource(
            const RenderingInterface& renderingInterface,
            bool keepCopy = false);

        NODISCARD TextureProxy* GetProxy() const
        {
            return m_textureProxy.get();
        }

    protected:
        uint8* Load_Impl(const std::string& path, size_t& out_size) override;

    protected:
        int32 m_width = 0;
        int32 m_height = 0;
        int32 m_channels = 0;
        PixelFormat m_format = PixelFormat::None;
        DataType m_componentType = DataType::None;
        TextureWrapMode m_wrapModeS = TextureWrapMode::Clamp;
        TextureWrapMode m_wrapModeT = TextureWrapMode::Clamp;
        TextureFilterMode m_filterMin = TextureFilterMode::Linear;
        TextureFilterMode m_filterMag = TextureFilterMode::Linear;
        uint32 m_numMips = 0;
        Ptr<TextureProxy> m_textureProxy;
    };

    class TextureManager final : public ResourceManager<Texture>
    {
    public:
    };
}