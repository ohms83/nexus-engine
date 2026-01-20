//
// Created by nutta on 7/25/2025.
//

#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/core/Color.h"
#include "nexus/ecs/Component.h"

NXS_NAMESPACE
{
    struct AmbientLightComponent : public IComponent
    {
        IMPLEMENT_REFLECTION(AmbientLightComponent);

        ComponentID GetComponentID() const override
        {
            return COMPONENT_HASH(AmbientLightComponent);
        }

        void AcceptReflector(IReflector& reflector) override
        {
            reflector.ChangeCategory("Light");
            reflector.VisitProperty("Ambient Color", typeid(Color3F), &color);
        }

        Color3F color {};
    };

    struct LightProperties : public IComponent
    {
        IMPLEMENT_REFLECTION(LightProperties);

        ComponentID GetComponentID() const override
        {
            return COMPONENT_HASH(LightProperties);
        }

        void AcceptReflector(IReflector& reflector) override
        {
            reflector.ChangeCategory("Light");
            reflector.VisitProperty("Color", typeid(Color3F), &color);
            reflector.VisitProperty("Diffuse Intensity", typeid(float), &diffuseIntensity);
            reflector.VisitProperty("Specular Intensity", typeid(float), &specularIntensity);
        }

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

    struct DirectLightComponent : public IComponent
    {
        IMPLEMENT_REFLECTION(DirectLightComponent);

        ComponentID GetComponentID() const override
        {
            return COMPONENT_HASH(DirectLightComponent);
        }

        void AcceptReflector(IReflector& reflector) override
        {
            properties.AcceptReflector(reflector);

            reflector.ChangeCategory("Light");
            reflector.VisitProperty("Direction", typeid(glm::vec3), &direction);
        }

        LightProperties properties;
        glm::vec3 direction;
    };

    struct PointLightComponent : public IComponent
    {
        IMPLEMENT_REFLECTION(PointLightComponent);

        ComponentID GetComponentID() const override
        {
            return COMPONENT_HASH(PointLightComponent);
        }

        void AcceptReflector(IReflector& reflector) override
        {
            properties.AcceptReflector(reflector);

            reflector.ChangeCategory("Light");
            reflector.VisitProperty("Constant", typeid(float), &constant);
            reflector.VisitProperty("Linear", typeid(float), &linear);
            reflector.VisitProperty("Quadratic", typeid(float), &quadratic);
        }

        LightProperties properties;
        float constant = 1.0f;
        float linear = 0.1f;
        float quadratic = 0.05f;
    };
}
