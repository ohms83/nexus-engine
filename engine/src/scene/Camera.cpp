//
// Created by nutta on 7/8/2025.
//

#include "nexus/scene/Camera.h"

USING_NAMESPACE_NXS;

void Camera::SetProjection(const float fov, const float width, const float height, const float nearZ, const float farZ)
{
    m_fov = fov;
    m_width = width;
    m_height = height;
    m_nearZ = nearZ,
    m_farZ = farZ;
    m_projMtx = glm::perspective(glm::radians(fov), width / height, nearZ, farZ);
}

void Camera::SetOrthographic(const float width, const float height, const float nearZ, const float farZ)
{
    m_fov = 90;
    m_width = width;
    m_height = height;
    m_nearZ = nearZ,
    m_farZ = farZ;
    m_projMtx = glm::ortho(-width/2, width/2, -height, height, nearZ, farZ);
}

glm::mat4 Camera::GetViewMtx() const
{
    const auto worldOrientation = transform.GetOrient(Transform::Space::Global);
    const auto worldPosition = transform.GetPosition(Transform::Space::Global);
    auto viewMatrix = glm::mat4(worldOrientation);
    viewMatrix[3] = glm::vec4(worldPosition, 1);
    return glm::inverse(viewMatrix);
}