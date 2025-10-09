#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/graphics/Color.h"
#include "nexus/math/Math.h"
#include "nexus/core/LogDispatcher.h"
#include "nexus/ecs/component/scene/TransformComponent.h"
#include "nexus/ecs/component/scene/LightComponent.h"

#include "SceneNode.h"

NXS_NAMESPACE
{
    class ILight
    {
    public:
        virtual ~ILight() = default;
        virtual const Color3F& GetColor() const = 0;
        virtual Color3F GetDiffuseColor() const = 0;
        virtual Color3F GetSpecularColor() const = 0;
        virtual float GetDiffuseIntensity() const = 0;
        virtual float GetSpecularIntensity() const = 0;
        virtual float GetCutoffRange() const = 0;
    };

    class DirectionalLight final : public ILight, public SceneNode
    {
    public:
        explicit DirectionalLight(entt::registry& registry);
        explicit DirectionalLight(entt::registry& registry, std::string  name);
        ~DirectionalLight() override = default;

        const Color3F& GetColor() const override;
        void SetColor(const Color3F& color);

        Color3F GetDiffuseColor() const override;
        Color3F GetSpecularColor() const override;

        float GetDiffuseIntensity() const override;
        void SetDiffuseIntensity(float intensity);
        float GetSpecularIntensity() const override;
        void SetSpecularIntensity(float intensity);

        float GetCutoffRange() const override
        {
            return NXS_INFINITE;
        }

        LightProperties& Properties()
        {
            return GetComponent<DirectLightComponent>().properties;
        }

        const LightProperties& Properties() const
        {
            return GetComponent<DirectLightComponent>().properties;
        }

        glm::vec3& Direction()
        {
            return GetComponent<DirectLightComponent>().direction;
        }

        const glm::vec3& Direction() const
        {
            return GetComponent<DirectLightComponent>().direction;
        }
    };

    class PointLight final : public ILight, public SceneNode
    {
    public:
        explicit PointLight(entt::registry& registry);
        explicit PointLight(entt::registry& registry, std::string  name);
        ~PointLight() override = default;

        const Color3F& GetColor() const override;
        void SetColor(const Color3F& color);

        Color3F GetDiffuseColor() const override;
        Color3F GetSpecularColor() const override;

        float GetDiffuseIntensity() const override;
        void SetDiffuseIntensity(float intensity);
        float GetSpecularIntensity() const override;
        void SetSpecularIntensity(float intensity);

        float GetCutoffRange() const override;
        void SetCutoffRange(float range);

        LightProperties& Properties()
        {
            return GetComponent<PointLightComponent>().properties;
        }

        const LightProperties& Properties() const
        {
            return GetComponent<PointLightComponent>().properties;
        }

        float GetConstantAttenuation() const;
        void SetConstantAttenuation(float attenuation);
        float GetLinearAttenuation() const;
        void SetLinearAttenuation(float attenuation);
        float GetQuadraticAttenuation() const;
        void SetQuadraticAttenuation(float attenuation);

        glm::vec3& Position()
        {
            return GetComponent<PositionComponent>().value;
        }

        const glm::vec3& Position() const
        {
            return GetComponent<PositionComponent>().value;
        }
    };
}