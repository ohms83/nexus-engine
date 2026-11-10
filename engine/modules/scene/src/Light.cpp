//
// Created by nutta on 8/17/2025.
//
#include "nexus/scene/Light.h"
#include "nexus/math/Matrix.h"

USING_NAMESPACE_NXS;

#define DEFAULT_CUTOFF 10.f

static uint32_t s_numLight = 0;

DirectionalLight::DirectionalLight(Ref<entt::registry>  registry, std::string name)
    : SceneNode(registry, name)
{
    if (name.empty())
    {
        name = std::format("DirectionalLight_{}", s_numLight++);
        SetName(name);
    }
    AddComponent<DirectLightComponent>();
}

PointLight::PointLight(Ref<entt::registry>  registry, std::string name)
    : SceneNode(registry, name)
{
    if (name.empty())
    {
        name = std::format("PointLight_{}", s_numLight++);
        SetName(name);
    }

    auto [lightComp, position] = AddComponents<PointLightComponent, PositionComponent>();
    lightComp->properties.cutoffRange = DEFAULT_CUTOFF;
}

glm::mat4 PointLight::GetLocalTransform() const
{
    return Matrix::CreateModelMatrix(
        GetPosition(),
        glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f));
}
