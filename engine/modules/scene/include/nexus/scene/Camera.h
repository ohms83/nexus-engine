//
// Created by nutta on 7/8/2025.
//

#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/geom/Frustum.h"

#include "SceneNode.h"
#include "SceneView.h"
#include "component/CameraComponent.h"
#include "component/TransformComponent.h"

NXS_NAMESPACE
{
    struct Sphere;

    class Camera : public SceneNode, public ISceneView
    {
    public:
        explicit Camera(Ref<entt::registry> registry, const std::string& name = "");

        IMPLEMENT_NODE(Camera);
        void AcceptReflector(IReflector& reflector) override;

        CameraComponent& Properties() { return *GetComponent<CameraComponent>(); }
        const CameraComponent& Properties() const { return *GetComponent<CameraComponent>(); }

        /**
         * Setup camera's projection.
         * @param fov Field-of-view in degree.
         * @param width Fustrum's width.
         * @param height Fustrum's height.
         * @param nearZ Fustrum's near clipping plane.
         * @param farZ Fustrum's far clipping plane.
         */
        void SetProjection(float fov, float width, float height, float nearZ, float farZ);
        void SetOrthographic(float width, float height, float nearZ, float farZ);

        NODISCARD glm::mat4 GetViewMatrix() const override;

        NODISCARD glm::mat4 GetProjectionMatrix() const override
        {
            return m_projMtx;
        }

        NODISCARD glm::mat4 GetViewProjectionMatrix() const override;

        // NODISCARD bool IsObjectVisible(const Sphere &boundingSphere, const glm::mat4 &modelMtx, const glm::vec3 &scale) const override;
        NODISCARD Array<SceneNode*> GetVisibleNodes(const Scene& scene) const override;

        NODISCARD Frustum GetViewFrustum() const;

    protected:
        //! Projection matrix.
        glm::mat4 m_projMtx{1.0f};
    };
}
