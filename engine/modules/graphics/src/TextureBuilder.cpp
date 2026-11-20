#include "nexus/graphics/TextureBuilder.h"
#include "nexus/graphics/GpuResourceFactory.h"
#include "nexus/debug/Assert.h"

USING_NAMESPACE_NXS;

//! Begin building this texture
TextureBuilder& TextureBuilder::Begin(IGpuResourceFactory* resourceFactory, const TextureDescription& info)
{
    NXS_ASSERT(resourceFactory);
    m_texture = resourceFactory->CreateTextureProxy(info);
    m_texture->Bind();
    return *this;
}

TextureBuilder& TextureBuilder::LoadData(const uint8_t* data, uint32_t size)
{
    NXS_ASSERT_MSG(m_texture, "Invalid operation! The Begin function must be called first!");
    m_texture->LoadData(data, size);
    return *this;
}

TextureBuilder& TextureBuilder::LoadMipData(const uint8_t* data, uint32_t size, uint32_t mip)
{
    NXS_ASSERT_MSG(m_texture, "Invalid operation! The Begin function must be called first!");
    m_texture->LoadMipData(data, size, mip);
    return *this;
}

Ref<TextureProxy> TextureBuilder::End()
{
    NXS_ASSERT_MSG(m_texture, "Invalid operation! The Begin function must be called first!");
    m_texture->Unbind();
    return m_texture;
}