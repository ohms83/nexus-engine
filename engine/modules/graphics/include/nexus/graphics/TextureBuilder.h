#pragma once

#include "nexus/NxsDefine.h"
#include "TextureProxy.h"

NXS_NAMESPACE
{
    class IGpuResourceFactory;

    class TextureBuilder
    {
    public:
        TextureBuilder() = default;
        //! Begin building this texture
        TextureBuilder& Begin(IGpuResourceFactory* resourceFactory, const TextureDescription& info);
        TextureBuilder& LoadData(const uint8_t* data, uint32_t size);
        TextureBuilder& LoadMipData(const uint8_t* data, uint32_t size, uint32_t mip);
        Ref<TextureProxy> End();

    private:
        Ref<TextureProxy> m_texture;
    };
}