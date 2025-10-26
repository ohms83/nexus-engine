//
// Created by nutta on 7/8/2025.
//

#include "scene/Camera.h"

USING_NAMESPACE_NXS;

Camera::Camera(Ref<entt::registry>  registry)
    : SceneNode(registry)
{
    AddComponents<CameraProperties, PositionComponent, OrientationComponent, RotationComponent>();
}

Camera::Camera(Ref<entt::registry>  registry, const std::string& name)
    : SceneNode(registry, name)
{
    AddComponents<CameraProperties, PositionComponent, OrientationComponent, RotationComponent>();
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
    SceneNode::AcceptReflector(reflector);

    reflector.ChangeCatetory("Transform");
    reflector.VisitProperty("Position", typeid(glm::vec3), &Position().value);
    reflector.VisitProperty("Orient", typeid(OrientationComponent), &Orient());

    auto& properties = Properties();
    bool valueChanged = false;
    auto valueChangeCallback = [&valueChanged](void*) { valueChanged = true; };

    reflector.ChangeCatetory("View Fustrum");
    reflector.VisitPropertyWithFeedback("FOV", typeid(float), &properties.fov, valueChangeCallback);
    reflector.VisitPropertyWithFeedback("Near", typeid(float), &properties.nearZ, valueChangeCallback);
    reflector.VisitPropertyWithFeedback("Far", typeid(float), &properties.farZ, valueChangeCallback);
    reflector.VisitPropertyWithFeedback("Width", typeid(float), &properties.width, valueChangeCallback);
    reflector.VisitPropertyWithFeedback("Height", typeid(float), &properties.height, valueChangeCallback);

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
}
