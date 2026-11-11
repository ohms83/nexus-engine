#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/core/Color.h"
#include "nexus/debug/LogDispatcher.h"
#include "nexus/math/Math.h"

#include "SceneNode.h"
#include "component/TransformComponent.h"
#include "component/LightComponent.h"

NXS_NAMESPACE
{
    class ILight
    {
    public:
        virtual ~ILight() = default;
        NODISCARD virtual LightProperties& Properties() = 0;
        NODISCARD virtual const LightProperties& Properties() const = 0;
    };

    class DirectionalLight final : public ILight, public SceneNode
    {
    public:
        explicit DirectionalLight(Ref<entt::registry> registry, std::string name = "");
        ~DirectionalLight() override = default;

        IMPLEMENT_NODE(DirectionalLight);

        NODISCARD LightProperties& Properties() override
        {
            return GetComponent<DirectLightComponent>()->properties;
        }

        NODISCARD const LightProperties& Properties() const override
        {
            return GetComponent<DirectLightComponent>()->properties;
        }

        NODISCARD glm::vec3& Direction()
        {
            return GetComponent<DirectLightComponent>()->direction;
        }

        NODISCARD const glm::vec3& Direction() const
        {
            return GetComponent<DirectLightComponent>()->direction;
        }
    };

    class PointLight final : public ILight, public SceneNode
    {
    public:
        explicit PointLight(Ref<entt::registry> registry, std::string  name = "");
        ~PointLight() override = default;

        IMPLEMENT_NODE(PointLight);

        NODISCARD LightProperties& Properties() override
        {
            return GetComponent<PointLightComponent>()->properties;
        }

        NODISCARD const LightProperties& Properties() const override
        {
            return GetComponent<PointLightComponent>()->properties;
        }

        NODISCARD PointLightComponent& PointLightProperties()
        {
            return *GetComponent<PointLightComponent>();
        }

        NODISCARD const PointLightComponent& PointLightProperties() const
        {
            return *GetComponent<PointLightComponent>();
        }
    };
}