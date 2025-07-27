//
// Created by nutta on 7/25/2025.
//

#pragma once

#include "nexus/NxsDefine.h"
#include <nexus/NxsCommon.h>

NXS_NAMESPACE
{
    struct TransformComponent
    {
        glm::vec3 translation;
        glm::quat rotation;
        glm::vec3 scale{1, 1, 1};
    };

    struct PositionComponent
    {
        glm::vec3 value;
    };

    struct OrientationComponent
    {
        glm::quat value;
    };

    struct ScaleComponent
    {
        glm::vec3 value;
    };

    struct MoveComponent
    {
        glm::vec3 direction;
        float speed = 0;
    };

    struct RotationComponent
    {
        //! Rotation axis
        glm::vec3 axis;
        //! Rotation speed in degree per second
        float degree = 0;
    };
}