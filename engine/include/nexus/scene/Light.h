#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/graphics/Color.h"
#include "nexus/math/Math.h"
#include "nexus/ecs/component/scene/LightComponent.h"

#include "SceneNode.h"

NXS_NAMESPACE
{
    class ILight
    {
    public:
        virtual ~ILight() = default;
        virtual const Color3F& GetDiffuseColor() = 0;
        virtual const Color3F& GetSpecularColor() = 0;
        virtual const Color3F& GetEmissiveColor() = 0;
        virtual float GetCutoffRange() = 0;
    };

    class DirectionalLight final : public ILight, public SceneNode
    {
    public:
        explicit DirectionalLight(entt::registry& registry);
        explicit DirectionalLight(entt::registry& registry, std::string  name);
        ~DirectionalLight() override = default;

        const Color3F& GetDiffuseColor() override;
        void SetDiffuseColor(const Color3F& color);
        const Color3F& GetSpecularColor() override;
        void SetSpecularColor(const Color3F& color);
        const Color3F& GetEmissiveColor() override;
        void SetEmissiveColor(const Color3F& color);

        float GetCutoffRange() override
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

        const Color3F& GetDiffuseColor() override;
        void SetDiffuseColor(const Color3F& color);
        const Color3F& GetSpecularColor() override;
        void SetSpecularColor(const Color3F& color);
        const Color3F& GetEmissiveColor() override;
        void SetEmissiveColor(const Color3F& color);
        float GetCutoffRange() override;
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

    private:
        PointLightComponent& m_lightComponent;
    };
}