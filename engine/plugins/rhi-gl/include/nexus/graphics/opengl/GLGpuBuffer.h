//
// Created by nutta on 7/5/2025.
//

#pragma once

#include <array>
#include "nexus/graphics/GPUBuffer.h"
#include "NxsGL.h"

NXS_NAMESPACE
{
    inline GLenum NxsBufferUsageToGLenum(const BufferUsage usage)
    {
        switch(usage)
        {
        case BufferUsage::StaticDraw:
            return GL_STATIC_DRAW;
        case BufferUsage::StaticRead:
            return GL_STATIC_READ;
        case BufferUsage::StaticCopy:
            return GL_STATIC_COPY;
        case BufferUsage::DynamicDraw:
            return GL_DYNAMIC_DRAW;
        case BufferUsage::DynamicRead:
            return GL_DYNAMIC_READ;
        case BufferUsage::DynamicCopy:
            return GL_DYNAMIC_COPY;
        case BufferUsage::StreamDraw:
            return GL_STREAM_DRAW;
        case BufferUsage::StreamRead:
            return GL_STREAM_READ;
        case BufferUsage::StreamCopy:
            return GL_STREAM_COPY;
        default:
            assert(false);
            return GL_NONE;
        }
    }
}
