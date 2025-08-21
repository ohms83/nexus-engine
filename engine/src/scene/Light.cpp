//
// Created by nutta on 8/17/2025.
//
#include "nexus/scene/Light.h"

USING_NAMESPACE_NXS;

DirectionalLight::DirectionalLight(entt::registry& registry)
    : SceneNode(registry)
    , m_lightComponent(AddComponent<DirectLightComponent>())
{
}

DirectionalLight::DirectionalLight(entt::registry& registry, std::string name)
    : SceneNode(registry, name)
    , m_lightComponent(AddComponent<DirectLightComponent>())
{
}

const Color3F& DirectionalLight::GetDiffuseColor()
{
    const auto& component = GetComponent<DirectLightComponent>();
    return CAST<const Color3F&>(component.light.diffuseColor);
}

void DirectionalLight::SetDiffuseColor(const Color3F& color)
{
    auto& component = GetComponent<DirectLightComponent>();
    component.light.diffuseColor = color;
}

const Color3F& DirectionalLight::GetSpecularColor()
{
    auto& [light, direction] = GetComponent<DirectLightComponent>();
    return CAST<Color3F&>(light.specularColor);
}

void DirectionalLight::SetSpecularColor(const Color3F& color)
{
    auto& [light, direction] = GetComponent<DirectLightComponent>();
    light.specularColor = color;
}

const Color3F& DirectionalLight::GetEmissiveColor()
{
    auto& [light, direction] = GetComponent<DirectLightComponent>();
    return CAST<Color3F&>(light.emissiveColor);
}

void DirectionalLight::SetEmissiveColor(const Color3F& color)
{
    auto& [light, direction] = GetComponent<DirectLightComponent>();
    light.emissiveColor = color;
}

PointLight::PointLight(entt::registry& registry)
    : SceneNode(registry)
    , m_lightComponent(AddComponent<PointLightComponent>())
{
}

PointLight::PointLight(entt::registry& registry, std::string name)
    : SceneNode(registry, name)
    , m_lightComponent(AddComponent<PointLightComponent>())
{
}

const Color3F& PointLight::GetDiffuseColor()
{
    auto& component = GetComponent<PointLightComponent>();
    return CAST<Color3F&>(component.light.diffuseColor);
}

void PointLight::SetDiffuseColor(const Color3F& color)
{
    auto& component = GetComponent<PointLightComponent>();
    component.light.diffuseColor = color;
}

const Color3F& PointLight::GetSpecularColor()
{
    auto& component = GetComponent<PointLightComponent>();
    return CAST<Color3F&>(component.light.specularColor);
}

void PointLight::SetSpecularColor(const Color3F& color)
{
    auto& component = GetComponent<PointLightComponent>();
    component.light.specularColor = color;
}

const Color3F& PointLight::GetEmissiveColor()
{
    auto& component = GetComponent<PointLightComponent>();
    return CAST<Color3F&>(component.light.emissiveColor);
}

void PointLight::SetEmissiveColor(const Color3F& color)
{
    auto& component = GetComponent<PointLightComponent>();
    component.light.emissiveColor = color;
}

float PointLight::GetCutoffRange()
{
    const auto& component = GetComponent<PointLightComponent>();
    return component.light.cutoffRange;
}

void PointLight::SetCutoffRange(const float range)
{
    auto& component = GetComponent<PointLightComponent>();
    component.light.cutoffRange = range;
}

float PointLight::GetConstantAttenuation() const
{
    const auto& component = GetComponent<PointLightComponent>();
    return component.constant;
}

void PointLight::SetConstantAttenuation(const float attenuation)
{
    auto& component = GetComponent<PointLightComponent>();
    component.constant = attenuation;
}

float PointLight::GetLinearAttenuation() const
{
    const auto& component = GetComponent<PointLightComponent>();
    return component.linear;
}

void PointLight::SetLinearAttenuation(const float attenuation)
{
    auto& component = GetComponent<PointLightComponent>();
    component.linear = attenuation;
}

float PointLight::GetQuadraticAttenuation() const
{
    const auto& component = GetComponent<PointLightComponent>();
    return component.quadratic;
}

void PointLight::SetQuadraticAttenuation(const float attenuation)
{
    auto& component = GetComponent<PointLightComponent>();
    component.quadratic = attenuation;
}
