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
        /// @brief Record the total amount of time each material is switched to in 
        /// one frame. The number of switches should always be one for every material;
        /// otherwise, it indicates that material batching is not working properly.
        /// @param materialId The unique ID of the material being switched to.
        void RecordMaterialSwitching(uint32_t materialId);
        void RecordProgramDraw(uint32_t programHandle, uint32_t indexCount, uint32_t instanceCount);
        void RecordMaterialDraw(uint32_t materialId, uint32_t indexCount, uint32_t instanceCount);
        void RecordDrawCall(uint32_t indexCount, uint32_t instanceCount);
    }
}

