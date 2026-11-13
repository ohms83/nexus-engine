#pragma once

#include "Sphere.h"
#include "Box.h"
#include "Frustum.h"

NXS_NAMESPACE
{
    namespace Collider
    {
        extern bool IsInside(const glm::vec3& point, const Box& box);
        extern bool IsInside(const glm::vec3& point, const Sphere& sphere);
        extern bool IsInside(const glm::vec3& point, const Frustum& frustum);
        extern bool IsIntersect(const Box& a, const Box& b);
        extern bool IsIntersect(const Box& box, const Sphere& sphere);
        extern bool IsIntersect(const Box& box, const Frustum& frustum);
        extern bool IsIntersect(const Sphere& a, const Sphere& b);
        extern bool IsIntersect(const Sphere& sphere, const Frustum& frustum);
    }
}