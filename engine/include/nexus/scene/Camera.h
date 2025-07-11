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

        glm::mat4 GetViewMtx() const
        {
            return transform.GetMatrix();
        }
        const glm::mat4& GetProjectionMtx() const
        {
            return m_projMtx;
        }

    protected:
        glm::mat4 m_projMtx{1.0f};
    };
}
