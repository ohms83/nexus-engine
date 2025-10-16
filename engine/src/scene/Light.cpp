//
// Created by nutta on 8/17/2025.
//
#include "scene/Light.h"

USING_NAMESPACE_NXS;

#define DEFAULT_CUTOFF 10.f

DirectionalLight::DirectionalLight(entt::registry& registry)
    : SceneNode(registry)
{
    AddComponent<DirectLightComponent>();
}

DirectionalLight::DirectionalLight(entt::registry& registry, std::string name)
    : SceneNode(registry, name)
{
    AddComponent<DirectLightComponent>();
}

PointLight::PointLight(entt::registry& registry)
    : SceneNode(registry)
{
    auto [lightComp, position] = AddComponents<PointLightComponent, PositionComponent>();
    lightComp.properties.cutoffRange = DEFAULT_CUTOFF;
}

PointLight::PointLight(entt::registry& registry, std::string name)
    : SceneNode(registry, name)
{
    auto [lightComp, position] = AddComponents<PointLightComponent, PositionComponent>();
    lightComp.properties.cutoffRange = DEFAULT_CUTOFF;
}
