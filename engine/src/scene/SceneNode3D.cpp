#include "nexus/scene/SceneNode3D.h"

USING_NAMESPACE_NXS;

SceneNode3D::SceneNode3D(Ref<entt::registry> registry)
    : SceneNode(registry)
{
    AddComponents<PositionComponent, OrientationComponent, ScaleComponent>();
}

SceneNode3D::SceneNode3D(Ref<entt::registry> registry, std::string name)
    : SceneNode(registry, name)
{
    AddComponents<PositionComponent, OrientationComponent, ScaleComponent>();
}

void SceneNode3D::AcceptReflector(IReflector& reflector)
{
    SceneNode::AcceptReflector(reflector);

    reflector.ChangeCategory("Transform");
    reflector.VisitProperty("Position", typeid(glm::vec3), &Position().value);
    reflector.VisitProperty("Orient", typeid(OrientationComponent), &Orient());
    reflector.VisitProperty("Scale", typeid(glm::vec3), &Scale().value);
}

void SceneNode3D::LookAt(const glm::vec3& center, const glm::vec3& up)
{
    Orient().LookAt(Position().value, center, up);
}

glm::vec3 SceneNode3D::Right() const
{
    static const glm::vec3 right = {1, 0, 0};
    return Orient().quat * right;
}

glm::vec3 SceneNode3D::Up() const
{
    static const glm::vec3 up = {0, 1, 0};
    return Orient().quat * up;
}

glm::vec3 SceneNode3D::Forward() const
{
    static const glm::vec3 forward = {0, 0, -1};
    return Orient().quat * forward;
}