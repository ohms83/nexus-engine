#pragma once

#include "nexus/NxsCommon.h"

#include "Plane.h"

#include <array>

NXS_NAMESPACE
{
    struct CameraProperties;

    struct Frustum
    {
        Frustum() = default;
        
        std::array<Plane, 6> planes;

        bool IsPointInside(const glm::vec3& point) const;
        bool IsSphereInside(const glm::vec3& center, float radius) const;

        static Frustum CreateViewFrustum(const glm::mat4& clipMatrix);
        static Frustum CreateViewFrustum(const glm::vec3& cameraPos, const glm::quat& cameraOrient, float fov, float aspect, float nearZ, float farZ);
    };
}