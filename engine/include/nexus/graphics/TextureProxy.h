//
// Created by nutta on 7/9/2025.
//

#pragma once

#include <nexus/NxsDefine.h>

#include "GpuResource.h"

NXS_NAMESPACE
{
    enum class TextureWrapMode
    {
        None,
        Clamp,
        Repeat,
        MirroredRepeat,
        ClampToEdge,
        ClampToBorder,
        MirroredClampToEdge,
        Num
    };

    enum class TextureFilterMode
    {
        None,
        Nearest,
        Linear,
        NearestMipmapNearest,
        LinearMipmapLinear,
        NearestMipmapLinear,
        LinearMipmapNearest,
        Num
    };

    struct TextureDescription
    {
        int32_t width = 0;
        int32_t height = 0;
        int32_t channels = 4;
        PixelFormat format = PixelFormat::RGBA;
        DataType componentType = DataType::UByte;
        TextureWrapMode wrapModeS = TextureWrapMode::Repeat;
        TextureWrapMode wrapModeT = TextureWrapMode::Repeat;
        TextureFilterMode filterMin = TextureFilterMode::Linear;
        TextureFilterMode filterMag = TextureFilterMode::Linear;
        uint32_t numMips = 0;

        size_t GetBufferSize() const;
    };

    /**
     * A class represents a texture object residing on the GPU.
     */
    class TextureProxy : public IGpuResource
    {
    public:
        TextureProxy() = default;
        ~TextureProxy() override;

        NODISCARD uint32_t GetHandle() const override
        {
            return m_textureID;
        }

        //! Begin building this texture
        virtual TextureProxy& Begin(const TextureDescription& info);
        virtual TextureProxy& LoadData(const uint8_t* data, uint32_t size);
        virtual TextureProxy& LoadMipData(const uint8_t* data, uint32_t size, uint32_t mip);
        virtual void End();

    protected:
        uint32_t m_textureID = 0;
        int32_t m_width = 0;
        int32_t m_height = 0;
        int32_t m_channels = 0;
        PixelFormat m_format = PixelFormat::None;
        DataType m_componentType = DataType::None;
        TextureWrapMode m_wrapModeS = TextureWrapMode::Clamp;
        TextureWrapMode m_wrapModeT = TextureWrapMode::Clamp;
        TextureFilterMode m_filterMin = TextureFilterMode::Linear;
        TextureFilterMode m_filterMag = TextureFilterMode::Linear;
        uint32_t m_numMips = 0;
    };
}