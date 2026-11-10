#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/geom/Sphere.h"
#include "nexus/geom/Frustum.h"

#include "SceneView.h"

NXS_NAMESPACE
{
    class CameraView : public ISceneView
    {
    public:
        explicit CameraView(const CameraComponent& camera, const PositionComponent& position, const OrientationComponent& orientation);
        explicit CameraView(const CameraComponent& camera, const glm::vec3& position, const glm::quat& orientation);

        NODISCARD bool IsObjectVisible(const Sphere& boundingSphere, const glm::mat4& modelMtx, const glm::vec3& scale) const override;
        NODISCARD const glm::vec3& GetViewPos() const override;
        NODISCARD const glm::mat4& GetViewMatrix() const override;
        NODISCARD const glm::mat4& GetProjectionMatrix() const override;
        NODISCARD const glm::mat4& GetViewProjectionMatrix() const override;
    private:
        Frustum m_viewFrustum;
        glm::vec3 m_viewPos;
        glm::mat4 m_viewMatrix;
        glm::mat4 m_projectionMatrix;
        glm::mat4 m_viewProjectionMatrix;
    };
}