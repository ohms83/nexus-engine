#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/graphics/Color.h"
#include "nexus/math/Math.h"
#include "nexus/core/LogDispatcher.h"
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

        DirectLightComponent& GetLightComponent()
        {
            return m_lightComponent;
        }

        const DirectLightComponent& GetLightComponent() const
        {
            return m_lightComponent;
        }

    private:
        DirectLightComponent& m_lightComponent;
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

        PointLightComponent& GetLightComponent()
        {
            return m_lightComponent;
        }

        const PointLightComponent& GetLightComponent() const
        {
            return m_lightComponent;
        }

        float GetConstantAttenuation() const;
        void SetConstantAttenuation(float attenuation);
        float GetLinearAttenuation() const;
        void SetLinearAttenuation(float attenuation);
        float GetQuadraticAttenuation() const;
        void SetQuadraticAttenuation(float attenuation);

        glm::vec3& Position()
        {
            return m_position.value;
        }

        const glm::vec3& Position() const
        {
            return m_position.value;
        }

    private:
        PointLightComponent& m_lightComponent;
        PositionComponent& m_position;
    };
}