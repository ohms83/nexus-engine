//
// Created by nutta on 7/25/2025.
//

#pragma once

#include "nexus/NxsCommon.h"
#include "nexus/ecs/Component.h"
#include "nexus/math/Math.h"

NXS_NAMESPACE
{
    struct PositionComponent : public IComponent
    {
        IMPLEMENT_REFLECTION(PositionComponent);

        ComponentID GetComponentID() const override
        {
            return COMPONENT_HASH(PositionComponent);
        }

        void AcceptReflector(IReflector& reflector) override
        {
            reflector.ChangeCategory("Transform");
            reflector.VisitProperty("Position", typeid(glm::vec3), &value);
        }

        glm::vec3 value;

        void Translate(const glm::vec3& translation)
        {
            value += translation;
        }
    };

    /**
     * @struct OrientationComponent
     * @brief Represents an entity's 3D rotation and orientation, storing the quaternion
     * as the primary source of truth.
     *
     * This component handles orientation using a glm::quat for rotations, which avoids
     * issues like Gimbal Lock. The Euler angles are maintained solely for UI presentation
     * and must be modified only through the provided member functions.
     */
    struct OrientationComponent : public IComponent
    {
        IMPLEMENT_REFLECTION(OrientationComponent);

        ComponentID GetComponentID() const override
        {
            return COMPONENT_HASH(OrientationComponent);
        }

        void AcceptReflector(IReflector& reflector) override
        {
            reflector.ChangeCategory("Transform");
            reflector.VisitPropertyWithFeedback("Orient", typeid(glm::vec3), &euler, [this](void* new_value) {
                const auto radians = glm::radians(euler);
                quat = glm::quat(radians);
            });
        }

        /// @brief The Quaternion (main source of truth) used for all transformations.
        glm::quat quat;

        /**
         * @brief Euler angles in Pitch, Yaw, Roll order (in degrees).
         * @warning This value is **read-only** for external access and used only for
         * UI visualization (e.g., property windows). Direct modification
         * will create a desync with the primary @c quat and lead to errors.
         * Use @c FromEuler, @c Rotate, or @c LookAt to safely update the orientation.
         */
        glm::vec3 euler;

        /**
         * @brief Initializes the orientation from the specified Euler angles.
         *
         * Converts the input Euler angles to a quaternion and updates both members.
         *
         * @param angles A vector representing Pitch, Yaw, and Roll in degrees.
         */
        void FromEuler(const glm::vec3& angles)
        {
            euler = angles;
            quat = glm::quat(glm::radians(euler));
        }

        /**
         * @brief Applies a rotation of a given degree around a specific axis.
         *
         * The quaternion is rotated, and the Euler angles are re-calculated from the
         * resulting quaternion to ensure consistency.
         *
         * @param degree The rotation amount in degrees.
         * @param axis The vector representing the axis of rotation (must be normalized).
         */
        void Rotate(const float degree, const glm::vec3& axis)
        {
            // Checks for a zero vector.
            if (glm::all(glm::epsilonEqual(axis, glm::vec3(0), glm::vec3(FLT_EPSILON)))) return;

            quat = glm::rotate(quat, glm::radians(degree), axis);
            euler = glm::degrees(glm::eulerAngles(quat));
        }

        /**
         * @brief Applies a rotation defined by a set of Euler angles.
         *
         * The input Euler angles are converted to a quaternion and multiplied with
         * the existing quaternion to combine rotations.
         *
         * @param eulerAngles A vector representing the incremental Pitch, Yaw, and Roll in degrees.
         */
        void Rotate(const glm::vec3& eulerAngles)
        {
            quat *= glm::quat(glm::radians(eulerAngles));
            euler = glm::degrees(glm::eulerAngles(quat));
        }

        /**
         * @brief Sets the orientation to look from one point towards a target point.
         *
         * Uses @c glm::quatLookAt to calculate the new orientation quaternion.
         *
         * @param pos The position of the object.
         * @param center The world point the object should look towards.
         * @param up The up direction vector (e.g., glm::vec3(0, 1, 0)).
         */
        void LookAt(const glm::vec3& pos, const glm::vec3& center, const glm::vec3& up)
        {
            quat = glm::quatLookAt(glm::normalize(center - pos), up);
            euler = glm::degrees(glm::eulerAngles(quat));
        }
    };

    struct ScaleComponent : public IComponent
    {
        IMPLEMENT_REFLECTION(ScaleComponent);

        ComponentID GetComponentID() const override
        {
            return COMPONENT_HASH(ScaleComponent);
        }

        void AcceptReflector(IReflector& reflector) override
        {
            reflector.ChangeCategory("Transform");
            reflector.VisitProperty("Scale", typeid(glm::vec3), &value);
        }

        glm::vec3 value {1, 1, 1};
    };

    struct MoveComponent : public IComponent
    {
        IMPLEMENT_REFLECTION(MoveComponent);

        ComponentID GetComponentID() const override
        {
            return COMPONENT_HASH(MoveComponent);
        }

        void AcceptReflector(IReflector& reflector) override
        {
            reflector.ChangeCategory("Move Component");
            reflector.VisitReadOnlyProperty("Direction", typeid(glm::vec3), &direction);
            reflector.VisitProperty("Speed", typeid(float), &speed);
        }

        glm::vec3 direction;
        float speed = 0;
    };

    struct RotationComponent : public IComponent
    {
        IMPLEMENT_REFLECTION(RotationComponent);

        ComponentID GetComponentID() const override
        {
            return COMPONENT_HASH(RotationComponent);
        }
        
        void AcceptReflector(IReflector& reflector) override
        {
            reflector.ChangeCategory("Rotation Component");
            reflector.VisitProperty("Axis", typeid(glm::vec3), &axis);
            reflector.VisitProperty("Degree", typeid(float), &degree);
        }

        //! Rotation axis
        glm::vec3 axis;
        //! Rotation speed in degree per second
        float degree = 0;
    };
}