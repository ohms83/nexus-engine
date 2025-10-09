//
// Created by nutta on 8/17/2025.
//
#include "scene/Light.h"

USING_NAMESPACE_NXS;

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

const Color3F &DirectionalLight::GetColor() const
{
    const auto& [properties, direction] = GetComponent<DirectLightComponent>();
    return properties.color;
}

void DirectionalLight::SetColor(const Color3F &color)
{
    auto& component = GetComponent<DirectLightComponent>();
    component.properties.color = color;
}

Color3F DirectionalLight::GetDiffuseColor() const
{
    const auto& [properties, direction] = GetComponent<DirectLightComponent>();
    return CAST<Color3F>(properties.color * properties.diffuseIntensity);
}

Color3F DirectionalLight::GetSpecularColor() const
{
    const auto& [properties, direction] = GetComponent<DirectLightComponent>();
    return CAST<Color3F>(properties.color * properties.specularIntensity);
}

float DirectionalLight::GetDiffuseIntensity() const
{
    const auto& [properties, direction] = GetComponent<DirectLightComponent>();
    return properties.diffuseIntensity;
}

void DirectionalLight::SetDiffuseIntensity(float intensity)
{
    auto& component = GetComponent<DirectLightComponent>();
    component.properties.diffuseIntensity = intensity;
}

float DirectionalLight::GetSpecularIntensity() const
{
    const auto& [properties, direction] = GetComponent<DirectLightComponent>();
    return properties.specularIntensity;
}

void DirectionalLight::SetSpecularIntensity(float intensity)
{
    auto& component = GetComponent<DirectLightComponent>();
    component.properties.specularIntensity = intensity;
}

PointLight::PointLight(entt::registry& registry)
    : SceneNode(registry)
{
    // AddComponent<PointLightComponent>();
    // AddComponent<PositionComponent>();
    AddComponents<PointLightComponent, PositionComponent>();
}

PointLight::PointLight(entt::registry& registry, std::string name)
    : SceneNode(registry, name)
{
    AddComponent<PointLightComponent>();
    AddComponent<PositionComponent>();
}

const Color3F &PointLight::GetColor() const
{
    const auto& component = GetComponent<PointLightComponent>();
    return component.properties.color;
}

void PointLight::SetColor(const Color3F &color)
{
    auto& component = GetComponent<PointLightComponent>();
    component.properties.color = color;
}

Color3F PointLight::GetDiffuseColor() const
{
    const auto& component = GetComponent<PointLightComponent>();
    return CAST<Color3F>(component.properties.color * component.properties.diffuseIntensity);
}

Color3F PointLight::GetSpecularColor() const
{
    const auto& component = GetComponent<PointLightComponent>();
    return CAST<Color3F>(component.properties.color * component.properties.specularIntensity);
}

float PointLight::GetDiffuseIntensity() const
{
    const auto& component = GetComponent<PointLightComponent>();
    return component.properties.diffuseIntensity;
}

void PointLight::SetDiffuseIntensity(float intensity)
{
    auto& component = GetComponent<PointLightComponent>();
    component.properties.diffuseIntensity = intensity;
}

float PointLight::GetSpecularIntensity() const
{
    const auto& component = GetComponent<PointLightComponent>();
    return component.properties.specularIntensity;
}

void PointLight::SetSpecularIntensity(float intensity)
{
    auto& component = GetComponent<PointLightComponent>();
    component.properties.specularIntensity = intensity;
}

float PointLight::GetCutoffRange() const
{
    const auto& component = GetComponent<PointLightComponent>();
    return component.properties.cutoffRange;
}

void PointLight::SetCutoffRange(const float range)
{
    auto& component = GetComponent<PointLightComponent>();
    component.properties.cutoffRange = range;
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
