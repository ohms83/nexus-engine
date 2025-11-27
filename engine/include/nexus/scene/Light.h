#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/graphics/Color.h"
#include "nexus/math/Math.h"
#include "nexus/core/LogDispatcher.h"

#include "SceneNode.h"
#include "component/TransformComponent.h"
#include "component/LightComponent.h"

NXS_NAMESPACE
{
    class ILight
    {
    public:
        virtual ~ILight() = default;
        virtual LightProperties& Properties() = 0;
        virtual const LightProperties& Properties() const = 0;
    };

    class DirectionalLight final : public ILight, public SceneNode
    {
    public:
        explicit DirectionalLight(Ref<entt::registry> registry, std::string name = "");
        ~DirectionalLight() override = default;

        IMPLEMENT_REFLECTION(DirectionalLight);

        LightProperties& Properties() override
        {
            return GetComponent<DirectLightComponent>().properties;
        }

        const LightProperties& Properties() const override
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
        explicit PointLight(Ref<entt::registry> registry, std::string  name = "");
        ~PointLight() override = default;

        IMPLEMENT_REFLECTION(PointLight);

        LightProperties& Properties() override
        {
            return GetComponent<PointLightComponent>().properties;
        }

        const LightProperties& Properties() const override
        {
            return GetComponent<PointLightComponent>().properties;
        }

        PointLightComponent& PointLightProperties()
        {
            return GetComponent<PointLightComponent>();
        }

        const PointLightComponent& PointLightProperties() const
        {
            return GetComponent<PointLightComponent>();
        }

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