#pragma once

#include "nexus/NxsCommon.h"
#include "glm/gtx/intersect.hpp"

#include "Sphere.h"
#include "Plane.h"

NXS_NAMESPACE
{
    struct Ray
    {
        Ray() = default;
        explicit Ray(const glm::vec3& org, const glm::vec3& dir)
            : origin(org)
            , direction(dir)
        {}

        bool Intersect(const Sphere& sphere)
        {
            float tmpOut;
            return  glm::intersectRaySphere(origin, direction, sphere.center, sphere.RadiusSquare(), tmpOut);
        }

        bool Intersect(const Sphere& sphere, float& outDistance)
        {
            return  glm::intersectRaySphere(origin, direction, sphere.center, sphere.RadiusSquare(), outDistance);
        }

        bool Intersect(const Plane& plane)
        {
            float tmpOut;
            return glm::intersectRayPlane(origin, direction, plane.distance * plane.normal, plane.normal, tmpOut);
        }

        bool Intersect(const Plane& plane, float& outDistance)
        {
            return glm::intersectRayPlane(origin, direction, plane.distance * plane.normal, plane.normal, outDistance);
        }

        glm::vec3 origin;
        glm::vec3 direction;
    };
}