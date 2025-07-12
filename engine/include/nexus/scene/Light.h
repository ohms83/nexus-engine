#pragma once

#include <nexus/NxsDefine.h>
#include <nexus/graphics/Color.h>
#include <nexus/math/Math.h>
#include "SceneNode.h"

NXS_NAMESPACE
{
    class Light : public SceneNode
    {
    public:
        Color3F diffuseColor;
        Color3F specularColor;
        Color3F emissiveColor;
        float cutoffRange = NXS_INFINITE;
    };

    class DirectionalLight : public Light
    {
    public:
        glm::vec3 GetDirection() const
        {
            return -transform.GetPosition();
        }
    };
}