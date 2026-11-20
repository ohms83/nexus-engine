#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/memory/Buffer.h"
#include "nexus/container/Array.h"
#include "TextureProxy.h"

NXS_NAMESPACE
{
    class CubeTexture : public IGpuResource
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

        virtual void LoadData(Face face, Ref<IBuffer> data) = 0;
        virtual void LoadData(Array<Ref<IBuffer>> data) = 0;

    protected:
        TextureDescription m_desc {};
    };
}