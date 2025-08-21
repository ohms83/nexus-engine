//
// Created by nutta on 7/8/2025.
//

#include "nexus/scene/Camera.h"

#include "ecs/component/scene/TransformComponent.h"

USING_NAMESPACE_NXS;

Camera::Camera(entt::registry& registry)
    : SceneNode(registry)
    , m_camera(AddComponent<CameraComponent>())
    , m_position(AddComponent<PositionComponent>())
    , m_orient(AddComponent<OrientationComponent>())
{
    registry.emplace<RotationComponent>(m_entity);
}

Camera::Camera(entt::registry& registry, const std::string& name)
    : SceneNode(registry, name)
    , m_camera(AddComponent<CameraComponent>())
    , m_position(AddComponent<PositionComponent>())
    , m_orient(AddComponent<OrientationComponent>())
{
    registry.emplace<RotationComponent>(m_entity);
}

void Camera::SetProjection(const float fov, const float width, const float height, const float nearZ, const float farZ)
{
    auto& registry = GetRegistry();
    m_camera.fov = fov;
    m_camera.width = width;
    m_camera.height = height;
    m_camera.nearZ = nearZ,
    m_camera.farZ = farZ;
    m_camera.projectionType = ProjectionType::Perspective;
    m_projMtx = glm::perspective(glm::radians(fov), float(width) / float(height), nearZ, farZ);
}

void Camera::SetOrthographic(const float width, const float height, const float nearZ, const float farZ)
{
    auto& registry = GetRegistry();
    m_camera.fov = 90;
    m_camera.width = width;
    m_camera.height = height;
    m_camera.nearZ = nearZ,
    m_camera.farZ = farZ;
    m_camera.projectionType = ProjectionType::Orthographic;
    m_projMtx = glm::ortho(-width/2, width/2, -height/2, height/2, nearZ, farZ);
}

glm::mat4 Camera::GetViewMtx() const
{
    // TODO: Handle transform heirachy.
    const auto worldOrientation = m_orient.value;
    const auto worldPosition = m_position.value;
    auto viewMatrix = glm::mat4(worldOrientation);
    viewMatrix[3] = glm::vec4(worldPosition, 1);
    return glm::inverse(viewMatrix);
}
void Camera::LookAt(const glm::vec3 &center, const glm::vec3 &up)
{
    m_orient.value = glm::quatLookAt(glm::normalize(center - m_position.value), up);
}