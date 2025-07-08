//
// Created by nutta on 7/8/2025.
//

#include <nexus/scene/Camera.h>

USING_NAMESPACE_NXS;

void Camera::SetProjection(const float fov, const float width, const float height, const float nearZ, const float farZ)
{
    m_projMtx = glm::perspective(glm::radians(fov), width / height, nearZ, farZ);
}

void Camera::SetOrthographic(const float width, const float height, const float nearZ, const float farZ)
{
    m_projMtx = glm::ortho(-width/2, width/2, -height, height, nearZ, farZ);
}