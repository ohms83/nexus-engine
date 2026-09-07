#pragma once

#include "nexus/NxsDefine.h"
#include <cstdint>

NXS_NAMESPACE
{
    namespace GraphicsProfiler
    {
        void Enable(bool enable);
        void BeginFrame();
        void EndFrameAndReport();

        void RecordProgramBind(uint32_t programHandle);
        void RecordTextureBind(uint32_t textureHandle);
        void RecordDrawCall(uint32_t indexCount, uint32_t instanceCount);
        void RecordProgramDraw(uint32_t programHandle, uint32_t indexCount, uint32_t instanceCount);
        void RecordMaterialDraw(uint32_t materialId, uint32_t indexCount, uint32_t instanceCount);
    }
}

