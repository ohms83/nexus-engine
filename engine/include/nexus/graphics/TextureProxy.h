//
// Created by nutta on 7/9/2025.
//

#pragma once

#include <nexus/NxsDefine.h>

#include "GpuResrouce.h"

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

    struct TextureCreationInfo
    {
        int32 width = 0;
        int32 height = 0;
        int32 channels = 4;
        PixelFormat format = PixelFormat::RGBA;
        DataType componentType = DataType::UByte;
        TextureWrapMode wrapModeS = TextureWrapMode::Clamp;
        TextureWrapMode wrapModeT = TextureWrapMode::Clamp;
        TextureFilterMode filterMin = TextureFilterMode::Linear;
        TextureFilterMode filterMag = TextureFilterMode::Linear;
        uint32 numMips = 0;
    };

    /**
     * A class represents a texture object residing on the GPU.
     */
    class TextureProxy : public GpuResrouce
    {
    public:
        TextureProxy() = default;
        ~TextureProxy() override;

        //! Begin building this texture
        virtual TextureProxy& Begin(const TextureCreationInfo& info);
        virtual TextureProxy& LoadData(const uint8* data, uint32 size);
        virtual TextureProxy& LoadMipData(const uint8* data, uint32 size, uint32 mip);
        virtual void End();
        
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
    };
}