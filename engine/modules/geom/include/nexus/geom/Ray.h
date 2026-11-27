#pragma once

#include "nexus/NxsCommon.h"
#include "glm/gtx/intersect.hpp"

NXS_NAMESPACE
{
    struct Ray
    {
        Ray() = default;
        explicit Ray(const glm::vec3& org, const glm::vec3& dir)
            : origin(org)
            , direction(dir)
        {}

        glm::vec3 origin;
        glm::vec3 direction;
    };
}