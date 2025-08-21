//
// Created by nutta on 7/8/2025.
//

#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/ecs/component/scene/CameraComponent.h"

#include "SceneNode.h"

NXS_NAMESPACE
{
    class Camera : public SceneNode
    {
    public:
        explicit Camera(entt::registry& registry);
        explicit Camera(entt::registry& registry, const std::string& name);

        PositionComponent& Position() { return m_position; }
        const PositionComponent& Position() const { return m_position; }

        OrientationComponent& Orient() { return m_orient; }
        const OrientationComponent& Orient() const { return m_orient; }

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

        float GetFOV() const { return m_camera.fov; }
        float Getwidth() const { return m_camera.width; }
        float GetHeight() const { return m_camera.height; }
        float GetNearZ() const { return m_camera.nearZ; }
        float GetFarZ() const { return m_camera.farZ; }

    protected:
        CameraComponent& m_camera;
        PositionComponent& m_position;
        OrientationComponent& m_orient;
        //! Projection matrix.
        glm::mat4 m_projMtx{1.0f};
    };
}
