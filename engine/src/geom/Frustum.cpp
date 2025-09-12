#include "geom/Frustum.h"
#include "core/LogDispatcher.h"

#include "glm/gtc/matrix_access.hpp"

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
    NXS_ASSERT(radius > 0);
    for (const auto& plane : planes)
    {
        if (glm::dot(plane.normal, center) + plane.distance < -radius) return false;
    }

    return true;
}

Frustum Frustum::CreateViewFrustum(const glm::mat4& viewProjMatrix)
{
    Frustum frustum {};

    const glm::vec4& row0 = glm::row(viewProjMatrix, 0);
    const glm::vec4& row1 = glm::row(viewProjMatrix, 1);
    const glm::vec4& row2 = glm::row(viewProjMatrix, 2);
    const glm::vec4& row3 = glm::row(viewProjMatrix, 3);

    glm::vec4 rightPlane = row3 - row0;
    glm::vec4 leftPlane = row3 + row0;
    glm::vec4 bottomPlane = row3 + row1;
    glm::vec4 topPlane = row3 - row1;
    glm::vec4 farPlane = row3 - row2;
    glm::vec4 nearPlane = row3 + row2;

#define SET_PLANE(index, plane) \
    { \
        auto& f_plane = frustum.planes[index]; \
        f_plane.normal = glm::vec3(plane); \
        f_plane.distance = plane.w; \
        f_plane.normal = glm::normalize(f_plane.normal); \
    }

    SET_PLANE(0, rightPlane);
    SET_PLANE(1, leftPlane);
    SET_PLANE(2, topPlane);
    SET_PLANE(3, bottomPlane);
    SET_PLANE(4, nearPlane);
    SET_PLANE(5, farPlane);
#undef SET_PLANE
    return frustum;
}