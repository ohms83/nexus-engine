//
// Created by nutta on 7/25/2025.
//

#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/graphics/Color.h"

NXS_NAMESPACE
{
    struct AmbientLightComponent
    {
        Color3F color {};
    };

    struct LightProperties
    {
        Color3F color {};
        float diffuseIntensity = 1.f;
        float specularIntensity = 1.f;
        float cutoffRange = FLT_MAX;

        Color3F DiffuseColor()
        {
            return CAST<Color3F>(color * diffuseIntensity);
        }

        Color3F SpecularColor()
        {
            return CAST<Color3F>(color * specularIntensity);
        }
    };

    struct DirectLightComponent
    {
        LightProperties properties;
        glm::vec3 direction;
    };

    struct PointLightComponent
    {
        LightProperties properties;
        // glm::vec3 position;
        float constant = 0.0f;
        float linear = 0.1f;
        float quadratic = 0.05f;
    };
}
