#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/geom/Sphere.h"
#include "nexus/geom/Frustum.h"

#include "glm/glm.hpp"

NXS_NAMESPACE
{
    struct CameraComponent;
    struct PositionComponent;
    struct OrientationComponent;

    class SceneView
    {
    public:
        virtual ~SceneView() = default;

        virtual NODISCARD bool IsObjectVisible(const Sphere& boundingSphere, const glm::mat4& modelMtx, const glm::vec3& scale) const = 0;
        virtual NODISCARD const glm::mat4& GetViewMatrix() const = 0;
        virtual NODISCARD const glm::mat4& GetProjectionMatrix() const = 0;
        virtual NODISCARD const glm::mat4& GetViewProjectionMatrix() const = 0;
    };

    class CameraView : public SceneView
    {
    public:
        explicit CameraView(const CameraComponent& camera, const PositionComponent& pos, const OrientationComponent& orient);

        NODISCARD bool IsObjectVisible(const Sphere& boundingSphere, const glm::mat4& modelMtx, const glm::vec3& scale) const override;
        NODISCARD const glm::mat4& GetViewMatrix() const override;
        NODISCARD const glm::mat4& GetProjectionMatrix() const override;
        NODISCARD const glm::mat4& GetViewProjectionMatrix() const override;
    private:
        Frustum m_viewFrustum;
        glm::mat4 m_viewMatrix;
        glm::mat4 m_projectionMatrix;
        glm::mat4 m_viewProjectionMatrix;
    };
}