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

        bool IsPointInside(const glm::vec3& point) const;
        bool IsSphereInside(const glm::vec3& center, float radius) const;

        static Frustum CreateViewFrustum(const glm::mat4& viewProjectMtx);
    };
}