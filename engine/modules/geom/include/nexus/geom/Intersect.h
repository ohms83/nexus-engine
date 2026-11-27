#pragma once

#include "nexus/NxsCommon.h"

NXS_NAMESPACE
{
    struct Box;
    struct Sphere;
    struct Plane;
    struct Ray;
    struct Frustum;

    namespace Geom
    {
        NODISCARD extern bool IsInside(const glm::vec3& point, const Box& box);
        NODISCARD extern bool IsInside(const glm::vec3& point, const Sphere& sphere);
        NODISCARD extern bool IsInside(const glm::vec3& point, const Frustum& frustum);
        NODISCARD extern bool IsIntersect(const Box& a, const Box& b);
        NODISCARD extern bool IsIntersect(const Box& box, const Sphere& sphere);
        NODISCARD extern bool IsIntersect(const Box& box, const Frustum& frustum);
        NODISCARD extern bool IsIntersect(const Sphere& a, const Sphere& b);
        NODISCARD extern bool IsIntersect(const Sphere& sphere, const Frustum& frustum);

        NODISCARD extern bool Intersect(const Ray& ray, const Sphere& sphere);
        NODISCARD extern bool Intersect(const Sphere& sphere, float& outDistance);
        NODISCARD extern bool Intersect(const Plane& plane);
        NODISCARD extern bool Intersect(const Plane& plane, float& outDistance);
    }
}