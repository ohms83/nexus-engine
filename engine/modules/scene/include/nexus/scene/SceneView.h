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

    /**
     * @brief Abstract base class representing a view of a scene.
     *
     * A SceneView encapsulates the necessary information to determine visibility
     * of objects in the scene, including view and projection matrices, and the
     * view frustum. Derived classes should implement specific types of views,
     * such as camera views.
     */
    class SceneView
    {
    public:
        virtual ~SceneView() = default;

        NODISCARD virtual bool IsObjectVisible(const Sphere& boundingSphere, const glm::mat4& modelMtx, const glm::vec3& scale) const = 0;
        NODISCARD virtual const glm::vec3& GetViewPos() const = 0;
        NODISCARD virtual const glm::mat4& GetViewMatrix() const = 0;
        NODISCARD virtual const glm::mat4& GetProjectionMatrix() const = 0;
        NODISCARD virtual const glm::mat4& GetViewProjectionMatrix() const = 0;
    };

    class CameraView : public SceneView
    {
    public:
        explicit CameraView(const CameraComponent& camera, const PositionComponent& pos, const OrientationComponent& orient);

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