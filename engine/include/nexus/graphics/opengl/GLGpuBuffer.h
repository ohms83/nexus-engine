//
// Created by nutta on 7/5/2025.
//

#pragma once

#include <array>
#include "graphics/GPUBuffer.h"

NXS_NAMESPACE
{
    inline GLenum NxsBufferUsageToGLenum(const BufferUsage usage)
    {
        const std::array<GLenum, SIZE_CAST(BufferUsage::Num)> glUsages = {
            GL_STATIC_DRAW,
            GL_STATIC_READ,
            GL_STATIC_COPY,
            GL_DYNAMIC_DRAW,
            GL_DYNAMIC_READ,
            GL_DYNAMIC_COPY,
            GL_STREAM_DRAW,
            GL_STREAM_READ,
            GL_STREAM_COPY,
        };
        assert(usage != BufferUsage::Num);
        return glUsages[INT_CAST(usage)];
    }
}
