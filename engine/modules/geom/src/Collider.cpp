#include "nexus/geom/Collider.h"

#include "Remotery.h"

NXS_NAMESPACE
{
    namespace Collider
    {
        bool IsInside(const glm::vec3& point, const Box& box)
        {
            rmt_ScopedCPUSample(Collider_IsInside_PointBox, 0);
            if (glm::any(glm::lessThan(box.extent, glm::vec3(0.0f)))) return false;
            return glm::all(glm::greaterThanEqual(point, box.GetMin()))
                && glm::all(glm::lessThanEqual(point, box.GetMax()));
        }

        bool IsInside(const glm::vec3& point, const Sphere& sphere)
        {
            rmt_ScopedCPUSample(Collider_IsInside_PointSphere, 0);
            if (sphere.radius < 0.0f) return false;
            const auto offset = point - sphere.center;
            return glm::dot(offset, offset) <= sphere.RadiusSquare();
        }

        bool IsInside(const glm::vec3& point, const Frustum& frustum)
        {
            rmt_ScopedCPUSample(Collider_IsInside_PointFrustum, 0);
            for (const auto& plane : frustum.planes)
            {
                if (glm::dot(plane.normal, point) + plane.distance < 0) return false;
            }

            return true;
        }

        bool IsIntersect(const Box& a, const Box& b)
        {
            rmt_ScopedCPUSample(Collider_IsIntersect_BoxBox, 0);
            if (glm::any(glm::lessThanEqual(a.extent, glm::vec3(0.0f)))
                || glm::any(glm::lessThanEqual(b.extent, glm::vec3(0.0f))))
            {
                return false;
            }

            const auto centerDistance = glm::abs(a.center - b.center);
            const auto combinedExtent = a.extent + b.extent;
            return glm::all(glm::lessThanEqual(centerDistance, combinedExtent));
        }

        bool IsIntersect(const Box& box, const Sphere& sphere)
        {
            rmt_ScopedCPUSample(Collider_IsIntersect_BoxSphere, 0);
            if (sphere.radius < 0.0f || glm::any(glm::lessThan(box.extent, glm::vec3(0.0f))))
            {
                return false;
            }

            const auto closestPoint = glm::clamp(sphere.center, box.GetMin(), box.GetMax());
            const auto offset = sphere.center - closestPoint;
            return glm::dot(offset, offset) <= sphere.RadiusSquare();
        }

        bool IsIntersect(const Box& box, const Frustum& frustum)
        {
            rmt_ScopedCPUSample(Collider_IsIntersect_BoxFrustum, 0);
            if (glm::any(glm::lessThan(box.extent, glm::vec3(0.0f)))) return false;

            const auto min = box.GetMin();
            const auto max = box.GetMax();
            for (const auto& plane : frustum.planes)
            {
                const glm::vec3 positiveVertex(
                    plane.normal.x >= 0.0f ? max.x : min.x,
                    plane.normal.y >= 0.0f ? max.y : min.y,
                    plane.normal.z >= 0.0f ? max.z : min.z);

                if (glm::dot(plane.normal, positiveVertex) + plane.distance < 0.0f)
                {
                    return false;
                }
            }

            return true;
        }

        bool IsIntersect(const Sphere& a, const Sphere& b)
        {
            rmt_ScopedCPUSample(Collider_IsIntersect_SphereSphere, 0);
            if (a.radius < 0.0f || b.radius < 0.0f) return false;
            const auto combinedRadius = a.radius + b.radius;
            const auto centerOffset = a.center - b.center;
            return glm::dot(centerOffset, centerOffset) <= combinedRadius * combinedRadius;
        }

        bool IsIntersect(const Sphere& sphere, const Frustum& frustum)
        {
            rmt_ScopedCPUSample(Collider_IsIntersect_SphereFrustum, 0);
            if (sphere.radius <= 0.0f) return false;

            for (const auto& plane : frustum.planes)
            {
                const auto signedDistance = glm::dot(plane.normal, sphere.center) + plane.distance;
                if (signedDistance < -sphere.radius) return false;
            }

            return true;
        }
    }
}