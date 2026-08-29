#pragma once

#include "nexus/NxsCommon.h"

#include "Plane.h"

#include <array>

NXS_NAMESPACE
{
    struct Frustum
    {
        Frustum() = default;
        
        std::array<Plane, 6> planes;

        NODISCARD bool IsPointInside(const glm::vec3& point) const;
        NODISCARD bool IsSphereInside(const glm::vec3& center, float radius) const;
    };
}