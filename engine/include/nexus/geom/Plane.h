#pragma once

#include "nexus/NxsCommon.h"
#include "glm/gtx/intersect.hpp"

NXS_NAMESPACE
{
    struct Plane
    {
        Plane() = default;
        explicit Plane(const glm::vec3& pos, const glm::vec3& n)
            : position(pos)
            , normal(n)
        {}

        glm::vec3 position;
        glm::vec3 normal;
    };
}