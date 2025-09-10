#pragma once

#include "nexus/NxsCommon.h"

NXS_NAMESPACE
{
    struct Box
    {
        Box() = default;
        explicit Box(const glm::vec3& pos, const glm::vec3& ext)
            : position(pos)
            , extent(ext)
        {}

        glm::vec3 position;
        glm::vec3 extent;
    };
}