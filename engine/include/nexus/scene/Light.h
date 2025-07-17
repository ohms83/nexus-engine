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

    class DirectionalLight final : public Light
    {
    public:
        glm::vec3 GetDirection() const
        {
            return -transform.GetPosition();
        }
    };

    class PointLight final : public Light
    {
    public:
        float constantAttenuation = 1.0f;
        float linearAttenuation = 0.1f;
        float quadraticAttenuation = 0.05f;
    };
}