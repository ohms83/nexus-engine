//
// Created by nutta on 8/17/2025.
//
#include "scene/Light.h"

USING_NAMESPACE_NXS;

#define DEFAULT_CUTOFF 10.f

DirectionalLight::DirectionalLight(Ref<entt::registry>  registry)
    : SceneNode(registry)
{
    AddComponent<DirectLightComponent>();
}

DirectionalLight::DirectionalLight(Ref<entt::registry>  registry, std::string name)
    : SceneNode(registry, name)
{
    AddComponent<DirectLightComponent>();
}

void DirectionalLight::AcceptReflector(IReflector& reflector)
{
    SceneNode::AcceptReflector(reflector);

    auto& properties = Properties();
    reflector.ChangeCategory("Light");
    reflector.VisitProperty("Color", typeid(Color3F), &properties.color);
    reflector.VisitProperty("Diffuse Intensity", typeid(float), &properties.diffuseIntensity);
    reflector.VisitProperty("Specular Intensity", typeid(float), &properties.specularIntensity);

    reflector.ChangeCategory("Direct Light");
    reflector.VisitProperty("Direction", typeid(glm::vec3), &Direction());
}

PointLight::PointLight(Ref<entt::registry>  registry)
    : SceneNode(registry)
{
    auto [lightComp, position] = AddComponents<PointLightComponent, PositionComponent>();
    lightComp.properties.cutoffRange = DEFAULT_CUTOFF;
}

PointLight::PointLight(Ref<entt::registry>  registry, std::string name)
    : SceneNode(registry, name)
{
    auto [lightComp, position] = AddComponents<PointLightComponent, PositionComponent>();
    lightComp.properties.cutoffRange = DEFAULT_CUTOFF;
}

void PointLight::AcceptReflector(IReflector& reflector)
{
    SceneNode::AcceptReflector(reflector);

    reflector.ChangeCategory("Transform");
    reflector.VisitProperty("Position", typeid(glm::vec3), &Position());

    auto& properties = Properties();
    reflector.ChangeCategory("Light");
    reflector.VisitProperty("Color", typeid(Color3F), &properties.color);
    reflector.VisitProperty("Diffuse Intensity", typeid(float), &properties.diffuseIntensity);
    reflector.VisitProperty("Specular Intensity", typeid(float), &properties.specularIntensity);
    reflector.VisitProperty("Cutoff", typeid(float), &properties.cutoffRange);

    auto& pointLight = PointLightProperties();
    reflector.ChangeCategory("Point Light");
    reflector.VisitProperty("Constant", typeid(float), &pointLight.constant);
    reflector.VisitProperty("Linear", typeid(float), &pointLight.linear);
    reflector.VisitProperty("Quadratic", typeid(float), &pointLight.quadratic);
}
