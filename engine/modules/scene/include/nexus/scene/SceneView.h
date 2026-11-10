#pragma once

#include "nexus/NxsDefine.h"

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
    class ISceneView
    {
    public:
        virtual ~ISceneView() = default;

        NODISCARD virtual bool IsObjectVisible(const Sphere& boundingSphere, const glm::mat4& modelMtx, const glm::vec3& scale) const = 0;
        NODISCARD virtual const glm::vec3& GetViewPos() const = 0;
        NODISCARD virtual const glm::mat4& GetViewMatrix() const = 0;
        NODISCARD virtual const glm::mat4& GetProjectionMatrix() const = 0;
        NODISCARD virtual const glm::mat4& GetViewProjectionMatrix() const = 0;
    };
}