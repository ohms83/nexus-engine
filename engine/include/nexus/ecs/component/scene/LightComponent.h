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
    struct LightComponent
    {
        Color3F diffuseColor {};
        Color3F specularColor {};
        Color3F emissiveColor {};
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
        float constant = 0.0f;
        float linear = 0.1f;
        float quadratic = 0.05f;
    };
}
