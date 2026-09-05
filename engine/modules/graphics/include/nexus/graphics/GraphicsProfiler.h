#pragma once

#include "nexus/NxsDefine.h"
#include <cstdint>

NXS_NAMESPACE
{
    namespace GraphicsProfiler
    {
        void BeginFrame();
        void EndFrameAndReport();

        void RecordProgramBind(uint32_t programHandle);
        void RecordTextureBind(uint32_t textureHandle);
        void RecordDrawCall(uint32_t indexCount, uint32_t instanceCount);
    }
}

