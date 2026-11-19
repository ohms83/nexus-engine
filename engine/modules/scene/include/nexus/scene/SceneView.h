#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/container/Array.h"
#include "nexus/scene/SceneNode.h"
#include "nexus/scene/RenderProfile.h"
#include "nexus/graphics/RenderTarget.h"

#include "glm/glm.hpp"

NXS_NAMESPACE
{
    struct Sphere;
    struct ViewContext;
    class Scene;
    class RenderingInterface;

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

    /**
     * @brief Snapshot of a view's rendering inputs for a scene.
     *
     * Created by ISceneView::CreateViewContext(), this context carries the
     * view's profile, transforms, render target, and visible nodes to the
     * scene renderer. Its pointers are non-owning: the view, target, and
     * visible nodes must remain alive while the context is used.
     */
    struct ViewContext
    {
        //! View that produced this context.
        const ISceneView* view = nullptr;
        //! Output target, or nullptr for the swapchain.
        RenderTarget* target = nullptr;
        //! Snapshot of the view's clear settings and render-pass selection.
        RenderProfile profile;

        //! View position in world space.
        glm::vec3 position;
        //! World-to-view transformation.
        glm::mat4 viewMatrix;
        //! View-to-clip transformation.
        glm::mat4 projectionMatrix;
        //! World-to-clip transformation (projectionMatrix * viewMatrix).
        glm::mat4 viewProjectionMatrix;

        //! Candidate visible nodes; individual passes may filter them further.
        Array<SceneNode*> visibleNodes;

        /**
         * @brief Clear the current render target according to the profile.
         *
         * Color and depth writes are enabled as needed and are not restored;
         * the next render pass applies its own state. Stencil clearing is not
         * currently supported by the generic rendering interface.
         * @param rhi Rendering interface with the desired target already bound.
         */
        void ClearView(RenderingInterface& rhi) const;
    };
}