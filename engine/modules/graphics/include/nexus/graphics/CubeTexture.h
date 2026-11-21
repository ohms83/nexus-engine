#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/memory/Buffer.h"
#include "nexus/container/Array.h"
#include "TextureProxy.h"

NXS_NAMESPACE
{
    class CubeTexture : public TextureProxy
    {
    public:
        enum class Face
        {
            PositiveX,
            NegativeX,
            PositiveY,
            NegativeY,
            PositiveZ,
            NegativeZ,
            Num
        };

        explicit CubeTexture(int32_t width, int32_t height);
        virtual ~CubeTexture() = default;

        void LoadData(const uint8_t* data, uint32_t size) override {}
        void LoadMipData(const uint8_t* data, uint32_t size, uint32_t mip)override {}

        virtual void LoadData(Face face, Ref<IBuffer> data) = 0;
        virtual void LoadData(Array<Ref<IBuffer>> data) = 0;
    };
}