//
// Created by nutta on 7/8/2025.
//

#pragma once

#include <nexus/NxsDefine.h>
#include "SceneNode.h"

NXS_NAMESPACE
{
    class Camera : public SceneNode
    {
    public:
        Camera() = default;
        ~Camera() = default;

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

        glm::mat4 GetViewMtx() const;
        const glm::mat4& GetProjectionMtx() const
        {
            return m_projMtx;
        }

        float GetFOV() const { return m_fov; }
        float Getwidth() const { return m_width; }
        float GetHeight() const { return m_height; }
        float GetNearZ() const { return m_nearZ; }
        float GetFarZ() const { return m_farZ; }

    protected:
        /**
         * Camera's field-of-view in degree. This will always be 90 in case of the
         * Orthographic projection.
         */
        float m_fov = 0;
        //! Fustrum's width.
        float m_width = 0;
        //! Fustrum's height.
        float m_height = 0;
        //! Fustrum's near clipping plane.
        float m_nearZ = 0;
        //! Fustrum's far clipping plane.
        float m_farZ = 0;
        //! Projection matrix.
        glm::mat4 m_projMtx{1.0f};
    };
}
