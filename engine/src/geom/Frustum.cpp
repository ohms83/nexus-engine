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

Frustum Frustum::CreateViewFrustum(const glm::mat4& clipMatrix)
{
    Frustum frustum {};

//     const glm::vec4& row0 = glm::row(clipMatrix, 0);
//     const glm::vec4& row1 = glm::row(clipMatrix, 1);
//     const glm::vec4& row2 = glm::row(clipMatrix, 2);
//     const glm::vec4& row3 = glm::row(clipMatrix, 3);

//     glm::vec4 rightPlane = row3 - row0;
//     glm::vec4 leftPlane = row3 + row0;
//     glm::vec4 bottomPlane = row3 + row1;
//     glm::vec4 topPlane = row3 - row1;
//     glm::vec4 farPlane = row3 - row2;
//     glm::vec4 nearPlane = row3 + row2;

// #define SET_PLANE(index, plane) \
//     { \
//         auto& f_plane = frustum.planes[index]; \
//         f_plane.normal = glm::vec3(plane); \
//         f_plane.distance = plane.w; \
//         f_plane.normal = glm::normalize(f_plane.normal); \
//     }

//     SET_PLANE(0, rightPlane);
//     SET_PLANE(1, leftPlane);
//     SET_PLANE(2, topPlane);
//     SET_PLANE(3, bottomPlane);
//     SET_PLANE(4, nearPlane);
//     SET_PLANE(5, farPlane);
// #undef SET_PLANE

    // Accessing matrix rows: GLM is column-major, so clipMatrix[i] is the i-th column.
    // To get the row, you must access elements directly or use the transpose.
    // For simplicity, we access the elements (M_i,j) by the standard mathematical convention (row i, col j)
    
    // Rows of the matrix (0-indexed)
    const glm::vec4 r1 = glm::row(clipMatrix, 0); // M_1j in formula (Col 0 in GLM)
    const glm::vec4 r2 = glm::row(clipMatrix, 1); // M_2j in formula (Col 1 in GLM)
    const glm::vec4 r3 = glm::row(clipMatrix, 2); // M_3j in formula (Col 2 in GLM)
    const glm::vec4 r4 = glm::row(clipMatrix, 3); // M_4j in formula (Col 3 in GLM)

    // Note: GLM matrices are column-major, so clipMatrix[col] returns the column vector.
    // To match the formula above (which assumes a row-major matrix), we will access the columns 
    // and treat them as the rows r1, r2, r3, r4.

    // Left Plane: r4 + r1
    frustum.planes[0] = Plane(r4 + r1);
    // Right Plane: r4 - r1
    frustum.planes[1] = Plane(r4 - r1);
    // Bottom Plane: r4 + r2
    frustum.planes[2] = Plane(r4 + r2);
    // Top Plane: r4 - r2
    frustum.planes[3] = Plane(r4 - r2);
    // Near Plane: r4 + r3
    frustum.planes[4] = Plane(r4 + r3);
    // Far Plane: r4 - r3
    frustum.planes[5] = Plane(r4 - r3);
    return frustum;
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