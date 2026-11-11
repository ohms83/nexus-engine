#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/container/Array.h"
#include "nexus/scene/SceneNode.h"

#include "glm/glm.hpp"

NXS_NAMESPACE
{
    struct Sphere;
    class Scene;

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

        NODISCARD const SceneNode* GetViewNode() const { return D_CAST<const SceneNode*>(this); }

        NODISCARD virtual bool IsObjectVisible(const Sphere& boundingSphere, const glm::mat4& modelMtx, const glm::vec3& scale) const { return false; };
        NODISCARD virtual glm::mat4 GetViewMatrix() const = 0;
        NODISCARD virtual glm::mat4 GetProjectionMatrix() const = 0;
        NODISCARD virtual glm::mat4 GetViewProjectionMatrix() const = 0;
        NODISCARD virtual Array<SceneNode*> GetVisibleNodes(const Scene& scene) const = 0;
    };
}