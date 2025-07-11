#pragma once

#include <nexus/NxsDefine.h>
#include "Light.h"

NXS_NAMESPACE
{
    class DirectionalLight : public Light
    {
    public:
        glm::vec3 GetDirection() const
        {
            return transform.GetLookVector();
        }
    };
}