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

void DirectionalLight::AcceptReflector(IReflector& reflector)
{
    SceneNode::AcceptReflector(reflector);

    auto& properties = Properties();
    reflector.ChangeCatetory("Light");
    reflector.VisitProperty("Color", typeid(Color3F), &properties.color);
    reflector.VisitProperty("Diffuse Intensity", typeid(float), &properties.diffuseIntensity);
    reflector.VisitProperty("Specular Intensity", typeid(float), &properties.specularIntensity);

    reflector.ChangeCatetory("Direct Light");
    reflector.VisitProperty("Direction", typeid(glm::vec3), &Direction());
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

void PointLight::AcceptReflector(IReflector& reflector)
{
    SceneNode::AcceptReflector(reflector);

    reflector.ChangeCatetory("Transform");
    reflector.VisitProperty("Position", typeid(glm::vec3), &Position());

    auto& properties = Properties();
    reflector.ChangeCatetory("Light");
    reflector.VisitProperty("Color", typeid(Color3F), &properties.color);
    reflector.VisitProperty("Diffuse Intensity", typeid(float), &properties.diffuseIntensity);
    reflector.VisitProperty("Specular Intensity", typeid(float), &properties.specularIntensity);
    reflector.VisitProperty("Cutoff", typeid(float), &properties.cutoffRange);

    auto& pointLight = PointLightProperties();
    reflector.ChangeCatetory("Point Light");
    reflector.VisitProperty("Constant", typeid(float), &pointLight.constant);
    reflector.VisitProperty("Linear", typeid(float), &pointLight.linear);
    reflector.VisitProperty("Quadratic", typeid(float), &pointLight.quadratic);
}
