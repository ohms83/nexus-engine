#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/container/Array.h"
#include "nexus/scene/SceneNode.h"
#include "nexus/graphics/RenderTarget.h"

#include "glm/glm.hpp"

NXS_NAMESPACE
{
    struct Sphere;
    class Scene;
    struct ViewContext;

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
        ISceneView();
        virtual ~ISceneView();

        NODISCARD const SceneNode* GetViewNode() const { return D_CAST<const SceneNode*>(this); }

        NODISCARD virtual bool IsObjectVisible(const Sphere& boundingSphere, const glm::mat4& modelMtx, const glm::vec3& scale) const { return false; };
        NODISCARD virtual glm::mat4 GetViewMatrix() const = 0;
        NODISCARD virtual glm::mat4 GetProjectionMatrix() const = 0;
        NODISCARD virtual glm::mat4 GetViewProjectionMatrix() const = 0;
        NODISCARD virtual Ref<RenderTarget> GetRenderTarget() const = 0;
        virtual void SetRenderTarget(Ref<RenderTarget> renderTarget) = 0;

        NODISCARD virtual ViewContext CreateViewContext(const Scene& scene) const = 0;

        static const Array<ISceneView*>& GetViewRegistry();
    };

    struct ViewContext
    {
        const ISceneView* view = nullptr;
        //! nullptr means swapchain
        RenderTarget* target = nullptr;

        // Clear
        ClearFlags clearFlags = ClearFlags::None;
        Color3F clearColor {0.0f, 0.0f, 0.0f};
        float clearDepth = 1.0f;
        uint32_t clearStencil = 0;

        //! Global view's position
        glm::vec3 position;
        glm::mat4 viewMatrix;
        glm::mat4 projectionMatrix;
        glm::mat4 viewProjectionMatrix;

        //! A list of visible scene nodes that will be rendered by this view.
        Array<SceneNode*> visibleNodes;
    };
}