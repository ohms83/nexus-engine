//
// Created by nutta on 7/8/2025.
//

#include "nexus/scene/Camera.h"

USING_NAMESPACE_NXS;

Camera::Camera(Ref<entt::registry>  registry, const std::string& name)
    : SceneNode(registry, name)
{
    AddComponents<CameraComponent, PositionComponent, OrientationComponent>();
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
    const auto worldOrientation = Orient().quat;
    const auto worldPosition = Position().value;
    auto viewMatrix = glm::mat4(worldOrientation);
    viewMatrix[3] = glm::vec4(worldPosition, 1);
    return glm::inverse(viewMatrix);
}

void Camera::LookAt(const glm::vec3 &center, const glm::vec3 &up)
{
    Orient().LookAt(Position().value, center, up);
}

void Camera::AcceptReflector(IReflector& reflector)
{
    bool valueChanged = false;

    // reflector.SetMarker("View Fustrum");
    auto connection = reflector.onValueChangedEvent.connect([&valueChanged](const std::string& name) {
        valueChanged = true;
    });

    SceneNode::AcceptReflector(reflector);

    auto& properties = Properties();
    if (valueChanged)
    {
        if (properties.projectionType == ProjectionType::Perspective)
        {
            SetProjection(properties.fov, properties.width, properties.height, properties.nearZ, properties.farZ);
        }
        else if (properties.projectionType == ProjectionType::Orthographic)
        {
            SetOrthographic(properties.width, properties.height, properties.nearZ, properties.farZ);
        }
    }

    reflector.onValueChangedEvent.disconnect(connection);
}
