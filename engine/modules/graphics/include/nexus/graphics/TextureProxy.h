//
// Created by nutta on 7/9/2025.
//

#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/memory/Buffer.h"

#include "GpuResource.h"

NXS_NAMESPACE
{
    enum class TextureKind
    {
        Texture2D,
        TextureCube,
    };

    enum class TextureTarget
    {
        Texture2D,
        PositiveX,
        NegativeX,
        PositiveY,
        NegativeY,
        PositiveZ,
        NegativeZ,
        Num,
    };

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

    /**
     * @brief Color encoding used when sampling a texture.
     *
     * sRGB textures are decoded to linear color values by the graphics API when sampled.
     * Data maps such as normal, roughness, metallic, and occlusion maps should remain linear.
     */
    enum class TextureColorSpace : uint32_t
    {
        Linear,
        SRgb,
    };

    struct TextureDescription
    {
        int32_t width = 0;
        int32_t height = 0;
        TextureKind kind = TextureKind::Texture2D;
        int32_t channels = 4;
        PixelFormat format = PixelFormat::RGBA;
        DataType componentType = DataType::UByte;
        //! Color interpretation for RGB/RGBA texture data.
        TextureColorSpace colorSpace = TextureColorSpace::Linear;
        TextureWrapMode wrapModeS = TextureWrapMode::Repeat;
        TextureWrapMode wrapModeT = TextureWrapMode::Repeat;
        TextureWrapMode wrapModeR = TextureWrapMode::Repeat;
        TextureFilterMode filterMin = TextureFilterMode::Linear;
        TextureFilterMode filterMag = TextureFilterMode::Linear;
        uint32_t numMips = 0;

        size_t GetBufferSize() const;

        inline bool IsValidDesc() const
        {
            return width > 0 && height > 0 && format != PixelFormat::None && channels >= 1;
        }

        static TextureDescription DepthTextureDesc(int32_t width, int32_t height);
        static TextureDescription DepthCubeTextureDesc(int32_t width, int32_t height);
    };

    /**
     * A class represents a texture object residing on the GPU.
     */
    class TextureProxy : public IGpuResource
    {
    public:
        using Target = TextureTarget;
        using Kind = TextureKind;
        using TextureType = TextureKind;

        explicit TextureProxy(const TextureDescription& desc);

        NODISCARD uint32_t GetHandle() const override
        {
            return m_textureID;
        }

        const TextureDescription& GetDescription() const { return m_desc; }

        virtual void LoadData(const uint8_t* data, uint32_t size) = 0;
        virtual void LoadData(TextureTarget target, const uint8_t* data, uint32_t size)
        {
            LoadData(data, size);
        }
        virtual void LoadMipData(const uint8_t* data, uint32_t size, uint32_t mip) = 0;
        virtual void GenerateMipmaps() {}
        virtual int32_t GetNumLevelMipmap() const { return m_desc.numMips; }
        virtual bool ReadTexel(uint32_t mip, void* data, size_t size) { return false; }
        virtual bool ReadTexels(uint32_t mip, Ref<IBuffer> outBuffer)  = 0;

    protected:
        uint32_t m_textureID = 0;
        TextureDescription m_desc {};
    };
}