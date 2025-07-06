//
// Created by nutta on 7/4/2025.
//

#pragma once

#include <nexus/NxsDefine.h>

NXS_NAMESPACE
{
    enum class BufferUsage
    {
        StaticDraw,
        StaticRead,
        StaticCopy,

        DynamicDraw,
        DynamicRead,
        DynamicCopy,

        StreamDraw,
        StreamRead,
        StreamCopy,

        Num
    };
}
