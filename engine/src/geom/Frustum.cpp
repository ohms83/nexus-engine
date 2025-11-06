#include "geom/Frustum.h"
#include "core/LogDispatcher.h"

#include "glm/gtc/matrix_access.hpp"
#include "glm/gtx/quaternion.hpp"

USING_NAMESPACE_NXS;

bool Frustum::IsPointInside(const glm::vec3& point) const
{
    for (const auto& plane : planes)
    {
        if (glm::dot(plane.normal, point) + plane.distance < 0) return false;
    }

    return true;
}

bool Frustum::IsSphereInside(const glm::vec3& center, float radius) const
{
    if(radius <= 0) return false;

    for (const auto& plane : planes)
    {
        const auto direction = glm::dot(plane.normal, center);
        const auto signedDistance = direction + plane.distance;
        if (signedDistance < -radius) {
            return false;
        }
    }

    return true;
}
