//
// Created by nutta on 7/8/2025.
//

#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/geom/Frustum.h"

#include "SceneNode.h"
#include "component/CameraComponent.h"
#include "component/TransformComponent.h"

NXS_NAMESPACE
{
    class Camera : public SceneNode
    {
    public:
        explicit Camera(Ref<entt::registry> registry, const std::string& name = "");

        IMPLEMENT_REFLECTION(Camera);
        void AcceptReflector(IReflector& reflector);

        CameraComponent& Properties() { return GetComponent<CameraComponent>(); }
        const CameraComponent& Properties() const { return GetComponent<CameraComponent>(); }

        NODISCARD PositionComponent& Position() { return GetComponent<PositionComponent>(); }
        NODISCARD const PositionComponent& Position() const { return GetComponent<PositionComponent>(); }

        NODISCARD OrientationComponent& Orient() { return GetComponent<OrientationComponent>(); }
        NODISCARD const OrientationComponent& Orient() const { return GetComponent<OrientationComponent>(); }

        void LookAt(const glm::vec3& center, const glm::vec3& up);

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

        NODISCARD glm::mat4 GetViewMtx() const;
        NODISCARD const glm::mat4& GetProjectionMtx() const
        {
            return m_projMtx;
        }

    protected:
        //! Projection matrix.
        glm::mat4 m_projMtx{1.0f};
    };
}
