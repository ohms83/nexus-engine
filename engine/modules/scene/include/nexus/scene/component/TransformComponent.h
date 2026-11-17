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
        IMPLEMENT_COMPONENT(PositionComponent);

        void AcceptReflector(IReflector& reflector) override
        {
            reflector.SetMarker("Transform");
            reflector.VisitVec3("Position", value);
        }
        
        VariantData Serialize() const override
        {
            auto data = Base::Serialize();
            data["position"] = value;
            return data;
        }

        MAYBE_UNUSED bool Deserialize(const VariantData& data) override
        {
            if (!Base::Deserialize(data)) return false;
            value = data["position"].GetVec3();
            return true;
        }

        glm::vec3 value {0, 0, 0};

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
    class OrientationComponent : public IComponent
    {
    public:
        IMPLEMENT_COMPONENT(OrientationComponent);

        void AcceptReflector(IReflector& reflector) override
        {
            reflector.SetMarker("Transform");
            if (reflector.VisitVec3("Orient", m_euler)) {
                const auto radians = glm::radians(m_euler);
                m_quat = glm::quat(radians);
            };
        }

        VariantData Serialize() const override
        {
            auto data = Base::Serialize();
            // Serialize Euler angles instead of Quaternion for human readability and UI purposes.
            data["orient"] = m_euler;
            return data;
        }

        MAYBE_UNUSED bool Deserialize(const VariantData& data) override
        {
            if (!Base::Deserialize(data)) return false;
            m_euler = data["orient"].GetVec3();
            m_quat = glm::quat(glm::radians(m_euler));
            return true;
        }

        const glm::quat& GetQuaternion() const { return m_quat; }
        void SetQuaternion(const glm::quat& quat)
        {
            m_quat = quat;
            m_euler = glm::degrees(glm::eulerAngles(m_quat));
        }

        const glm::vec3& GetAngles() const { return m_euler; }
        void SetAngles(const glm::vec3& angles)
        {
            m_euler = angles;
            m_quat = glm::quat(glm::radians(m_euler));
        }

        /**
         * @brief Initializes the orientation from the specified Euler angles.
         *
         * Converts the input Euler angles to a quaternion and updates both members.
         *
         * @param angles A vector representing Pitch, Yaw, and Roll in degrees.
         */
        void FromEuler(const glm::vec3& angles)
        {
            m_euler = angles;
            m_quat = glm::quat(glm::radians(m_euler));
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

            m_quat = glm::rotate(m_quat, glm::radians(degree), axis);
            m_euler = glm::degrees(glm::eulerAngles(m_quat));
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
            m_quat *= glm::quat(glm::radians(eulerAngles));
            m_euler = glm::degrees(glm::eulerAngles(m_quat));
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
            m_quat = glm::quatLookAt(glm::normalize(center - pos), up);
            m_euler = glm::degrees(glm::eulerAngles(m_quat));
        }

    private:
        /// @brief The Quaternion (main source of truth) used for all transformations.
        glm::quat m_quat = glm::quat(1, 0, 0, 0);
        /**
         * @brief Euler angles in Pitch, Yaw, Roll order (in degrees).
         * @warning This value is **read-only** for external access and used only for
         * UI visualization (e.g., property windows). Direct modification
         * will create a desync with the primary @c quat and lead to errors.
         * Use @c FromEuler, @c Rotate, or @c LookAt to safely update the orientation.
         */
        glm::vec3 m_euler {0, 0, 0};
    };

    struct ScaleComponent : public IComponent
    {
        IMPLEMENT_COMPONENT(ScaleComponent);

        void AcceptReflector(IReflector& reflector) override
        {
            reflector.SetMarker("Transform");
            reflector.VisitVec3("Scale", value);
        }

        VariantData Serialize() const override
        {
            auto data = Base::Serialize();
            data["scale"] = value;
            return data;
        }

        MAYBE_UNUSED bool Deserialize(const VariantData& data) override
        {
            if (!Base::Deserialize(data)) return false;
            value = data["scale"].GetVec3();
            return true;
        }

        glm::vec3 value {1, 1, 1};
    };

    struct MoveComponent : public IComponent
    {
        using Base = IComponent;

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
            auto data = Base::Serialize();
            data["direction"] = direction;
            data["speed"] = speed;
            return data;
        }

        MAYBE_UNUSED bool Deserialize(const VariantData& data) override
        {
            if (!Base::Deserialize(data)) return false;
            direction = data["direction"].GetVec3();
            speed = data["speed"].GetFloat();
            return true;
        }

        glm::vec3 direction {0, 0, 0};
        float speed = 0;
    };

    struct TurningComponent : public IComponent
    {
        using Base = IComponent;

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
            auto data = Base::Serialize();
            data["axis"] = axis;
            data["degree"] = degree;
            return data;
        }

        MAYBE_UNUSED bool Deserialize(const VariantData& data) override
        {
            if (!Base::Deserialize(data)) return false;
            axis = data["axis"].GetVec3();
            degree = data["degree"].GetFloat();
            return true;
        }

        //! Rotation axis
        glm::vec3 axis {0, 1, 0};
        //! Rotation speed in degree per second
        float degree = 0;
    };
}