#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/graphics/GraphicsConst.h"

NXS_NAMESPACE
{
    struct CameraProperties
    {
        ProjectionType projectionType = ProjectionType::Perspective;
        /**
         * Camera's field-of-view in degree. This will always be 90 in case of the
         * Orthographic projection.
         */
        float fov = 45.0f;
        //! Fustrum's width.
        float width = 0;
        //! Fustrum's height.
        float height = 0;
        //! Fustrum's near clipping plane.
        float nearZ = 0.1f;
        //! Fustrum's far clipping plane.
        float farZ = 1000.0f;

        float GetAspect() const { return height == 0 ? 0 : width / height; };
    };
}