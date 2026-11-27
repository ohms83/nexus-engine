#pragma once

#include "nexus/NxsCommon.h"

NXS_NAMESPACE
{
    struct Rect
    {
        glm::vec2 pos;
        glm::vec2 size;
    };

    struct iRect
    {
        glm::ivec2 pos;
        glm::ivec2 size;
    };
}