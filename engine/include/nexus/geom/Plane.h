#pragma once

#include "nexus/NxsCommon.h"
#include "glm/gtx/intersect.hpp"

NXS_NAMESPACE
{
    struct Plane
    {
        Plane() = default;
        explicit Plane(const glm::vec3& n, float d)
            : normal(n)
            , distance(d)
        {}

        glm::vec3 normal {};
        float distance = 0;
    };
}