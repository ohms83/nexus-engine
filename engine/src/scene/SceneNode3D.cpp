#include "nexus/scene/SceneNode3D.h"

USING_NAMESPACE_NXS;

SceneNode3D::SceneNode3D(entt::registry &registry)
    : SceneNode(registry)
    , m_position(AddComponent<PositionComponent>())
    , m_orient(AddComponent<OrientationComponent>())
    , m_scale(AddComponent<ScaleComponent>())
{
}

SceneNode3D::SceneNode3D(entt::registry &registry, std::string name)
    : SceneNode(registry, name)
    , m_position(AddComponent<PositionComponent>())
    , m_orient(AddComponent<OrientationComponent>())
    , m_scale(AddComponent<ScaleComponent>())
{
}

void SceneNode3D::LookAt(const glm::vec3& center, const glm::vec3& up)
{
    m_orient.value = glm::quatLookAt(glm::normalize(center - m_position.value), up);
}

glm::vec3 SceneNode3D::Right() const
{
    glm::vec3 right = {1, 0, 0};
    return Orient().value * right;
}

glm::vec3 SceneNode3D::Up() const
{
    glm::vec3 up = {0, 1, 0};
    return Orient().value * up;
}

glm::vec3 SceneNode3D::Forward() const
{
    glm::vec3 forward = {0, 0, -1};
    return Orient().value * forward;
}