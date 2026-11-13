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
        
        float RadiusSquare() const
        {
            return radius * radius;
        }
        
        glm::vec3 center;
        float radius = 0;
    };
    
}