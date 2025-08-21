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

        void Translate(const glm::vec3& translation)
        {
            value += translation;
        }
    };

    struct OrientationComponent
    {
        glm::quat value;

        void Rotate(const float degree, const glm::vec3& axis)
        {
            value = glm::rotate(value, glm::radians(degree), axis);
        }

        void Rotate(const glm::vec3& eulerAngles)
        {
            value *= glm::quat(glm::radians(eulerAngles));
        }
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