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
        Color3F color;
    };
    struct LightComponent
    {
        glm::vec3 diffuseColor;
        glm::vec3 specularColor;
        glm::vec3 emissiveColor;
        float cutoffRange = FLT_MAX;
    };

    struct DirectLightComponent
    {
        LightComponent light;
        glm::vec3 direction;
    };

    struct PointLightComponent
    {
        LightComponent light;
        glm::vec3 position;
        float constantAttenuation = 1.0f;
        float linearAttenuation = 0.1f;
        float quadraticAttenuation = 0.05f;
    };
}
