#pragma once

#include "nexus/NxsCommon.h"

NXS_NAMESPACE
{
    struct Box
    {
        Box() = default;
        explicit Box(const glm::vec3& pos, const glm::vec3& ext)
            : center(pos)
            , extent(ext)
        {}

        glm::vec3 center;
        glm::vec3 extent;
    };
}