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
        using Super = IComponent;

        IMPLEMENT_COMPONENT(PositionComponent);

        void AcceptReflector(IReflector& reflector) override
        {
            reflector.SetMarker("Transform");
            reflector.VisitVec3("Position", value);
        }
        
        VariantData Serialize() const override
        {
            auto data = Super::Serialize();
            data["position"] = value;
            return data;
        }

        MAYBE_UNUSED bool Deserialize(const VariantData& data) override
        {
            if (!Super::Deserialize(data)) return false;
            value = data["position"].GetVec3();
            return true;
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
        using Super = IComponent;

        IMPLEMENT_COMPONENT(OrientationComponent);

        void AcceptReflector(IReflector& reflector) override
        {
            reflector.SetMarker("Transform");
            if (reflector.VisitVec3("Orient", euler)) {
                const auto radians = glm::radians(euler);
                quat = glm::quat(radians);
            };
        }

        VariantData Serialize() const override
        {
            auto data = Super::Serialize();
            data["orient"] = euler;
            return data;
        }

        MAYBE_UNUSED bool Deserialize(const VariantData& data) override
        {
            if (!Super::Deserialize(data)) return false;
            euler = data["orient"].GetVec3();
            quat = glm::quat(glm::radians(euler));
            return true;
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
        using Super = IComponent;

        IMPLEMENT_COMPONENT(ScaleComponent);

        void AcceptReflector(IReflector& reflector) override
        {
            reflector.SetMarker("Transform");
            reflector.VisitVec3("Scale", value);
        }

        VariantData Serialize() const override
        {
            auto data = Super::Serialize();
            data["scale"] = value;
            return data;
        }

        MAYBE_UNUSED bool Deserialize(const VariantData& data) override
        {
            if (!Super::Deserialize(data)) return false;
            value = data["scale"].GetVec3();
            return true;
        }

        glm::vec3 value {1, 1, 1};
    };

    struct MoveComponent : public IComponent
    {
        using Super = IComponent;

        IMPLEMENT_COMPONENT(MoveComponent);

        void AcceptReflector(IReflector& reflector) override
        {
            reflector.SetMarker("Movement");
            reflector.VisitFloat("Speed", speed);

            reflector.SetReadOnlyFlag(true);
            reflector.VisitVec3("Direction", direction);
            reflector.SetReadOnlyFlag(false);
        }

        VariantData Serialize() const override
        {
            auto data = Super::Serialize();
            data["direction"] = direction;
            data["speed"] = speed;
            return data;
        }

        MAYBE_UNUSED bool Deserialize(const VariantData& data) override
        {
            if (!Super::Deserialize(data)) return false;
            direction = data["direction"].GetVec3();
            speed = data["speed"].GetFloat();
            return true;
        }

        glm::vec3 direction;
        float speed = 0;
    };

    struct TurningComponent : public IComponent
    {
        using Super = IComponent;

        IMPLEMENT_COMPONENT(TurningComponent);
        
        void AcceptReflector(IReflector& reflector) override
        {
            reflector.SetMarker("Turning");
            reflector.VisitFloat("Degree", degree);

            reflector.SetReadOnlyFlag(true);
            reflector.VisitVec3("Axis", axis);
            reflector.SetReadOnlyFlag(false);
        }

        VariantData Serialize() const override
        {
            auto data = Super::Serialize();
            data["axis"] = axis;
            data["degree"] = degree;
            return data;
        }

        MAYBE_UNUSED bool Deserialize(const VariantData& data) override
        {
            if (!Super::Deserialize(data)) return false;
            axis = data["axis"].GetVec3();
            degree = data["degree"].GetFloat();
            return true;
        }

        //! Rotation axis
        glm::vec3 axis;
        //! Rotation speed in degree per second
        float degree = 0;
    };
}