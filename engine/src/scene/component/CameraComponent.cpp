#include "scene/component/CameraComponent.h"
#include "nexus/geom/Frustum.h"

USING_NAMESPACE_NXS;

Frustum CameraComponent::GetViewFrustum(const glm::vec3& cameraPos,const glm::quat& cameraOrient) const
{
    Frustum frustum {};
    const auto fovY = glm::radians(fov);
    const auto halfVSide = farZ * tanf(fovY * .5f);
    const auto halfHSide = halfVSide * GetAspect();
    const auto right = glm::rotate(cameraOrient, glm::vec3(1, 0, 0));
    const auto up = glm::rotate(cameraOrient, glm::vec3(0, 1, 0));
    const auto front = glm::rotate(cameraOrient, glm::vec3(0, 0, -1));
    const auto frontMultFar = farZ * front;

    frustum.planes[0] = Plane(front, cameraPos + nearZ * front);
    frustum.planes[1] = Plane(-front, cameraPos + frontMultFar);
    frustum.planes[2] = Plane(glm::cross(frontMultFar - right * halfHSide, up), cameraPos);
    frustum.planes[3] = Plane(glm::cross(up, frontMultFar + right * halfHSide), cameraPos);
    frustum.planes[4] = Plane(glm::cross(right, frontMultFar - up * halfVSide), cameraPos);
    frustum.planes[5] = Plane(glm::cross(frontMultFar + up * halfVSide, right), cameraPos);

    return frustum;
}