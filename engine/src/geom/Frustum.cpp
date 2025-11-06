#include "geom/Frustum.h"
#include "core/LogDispatcher.h"

#include "glm/gtc/matrix_access.hpp"
#include "glm/gtx/quaternion.hpp"

USING_NAMESPACE_NXS;

bool Frustum::IsPointInside(const glm::vec3& point) const
{
    for (const auto& plane : planes)
    {
        if (glm::dot(plane.normal, point) + plane.distance < 0) return false;
    }

    return true;
}

bool Frustum::IsSphereInside(const glm::vec3& center, float radius) const
{
    if(radius <= 0) return false;

    for (const auto& plane : planes)
    {
        const auto direction = glm::dot(plane.normal, center);
        const auto signedDistance = direction + plane.distance;
        if (signedDistance < -radius) {
            return false;
        }
    }

    return true;
}

Frustum Frustum::CreateViewFrustum(
    const glm::vec3& cameraPos,
    const glm::quat& cameraOrient,
    float fov,
    float aspect,
    float nearZ,
    float farZ)
{
    Frustum frustum {};
    const auto fovY = glm::radians(fov);
    const auto halfVSide = farZ * tanf(fovY * .5f);
    const auto halfHSide = halfVSide * aspect;
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