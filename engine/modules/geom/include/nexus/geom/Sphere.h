#pragma once

#include "nexus/NxsCommon.h"

NXS_NAMESPACE
{
    struct Sphere
    {
        Sphere() = default;
        Sphere(const glm::vec3 pos, float r)
            : center(pos)
            , radius(r)
        {}

        bool Intersect(const Sphere& rhs) const
        {
            return glm::distance(center,  rhs.center) <= radius + rhs.radius;
        }
        
        float RadiusSquare() const
        {
            return radius * radius;
        }
        
        glm::vec3 center;
        float radius = 0;
    };
    
}