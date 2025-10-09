//
// Created by nutta on 7/8/2025.
//

#include "scene/Camera.h"
#include "ecs/component/scene/TransformComponent.h"

USING_NAMESPACE_NXS;

Camera::Camera(entt::registry& registry)
    : SceneNode(registry)
{
    AddComponent<CameraProperties, PositionComponent, OrientationComponent, RotationComponent>();
}

Camera::Camera(entt::registry& registry, const std::string& name)
    : SceneNode(registry, name)
{
    AddComponent<CameraProperties, PositionComponent, OrientationComponent, RotationComponent>();
}

void Camera::SetProjection(const float fov, const float width, const float height, const float nearZ, const float farZ)
{
    auto& properties = Properties();
    properties.fov = fov;
    properties.width = width;
    properties.height = height;
    properties.nearZ = nearZ,
    properties.farZ = farZ;
    properties.projectionType = ProjectionType::Perspective;
    m_projMtx = glm::perspective(glm::radians(fov), float(width) / float(height), nearZ, farZ);
}

void Camera::SetOrthographic(const float width, const float height, const float nearZ, const float farZ)
{
    auto& properties = Properties();
    properties.fov = 90;
    properties.width = width;
    properties.height = height;
    properties.nearZ = nearZ,
    properties.farZ = farZ;
    properties.projectionType = ProjectionType::Orthographic;
    m_projMtx = glm::ortho(-width/2, width/2, -height/2, height/2, nearZ, farZ);
}

glm::mat4 Camera::GetViewMtx() const
{
    // TODO: Handle transform heirachy.
    const auto worldOrientation = Orient().value;
    const auto worldPosition = Position().value;
    auto viewMatrix = glm::mat4(worldOrientation);
    viewMatrix[3] = glm::vec4(worldPosition, 1);
    return glm::inverse(viewMatrix);
}

void Camera::LookAt(const glm::vec3 &center, const glm::vec3 &up)
{
    Orient().value = glm::quatLookAt(glm::normalize(center - Position().value), up);
}
