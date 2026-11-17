//
// Created by nutta on 7/25/2025.
//

#pragma once

#include "nexus/NxsCommon.h"
#include "nexus/ecs/Component.h"
#include "nexus/math/Math.h"
#include "nexus/math/Matrix.h"
#include "nexus/math/Quaterniion.h"
#include <glm/gtx/matrix_decompose.hpp>

NXS_NAMESPACE
{
    struct MoveComponent : public IComponent
    {
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

    class TransformComponent final : public IComponent
    {
    public:
        IMPLEMENT_COMPONENT(TransformComponent)

        // Keep the signal and its connections at a stable address in EnTT storage.
        static constexpr bool in_place_delete = true;

        enum class ComponentType : int8_t
        {
            Position,
            Orientation,
            Scale
        };

        TransformComponent() = default;
        ~TransformComponent() override = default;

        void AcceptReflector(IReflector& reflector) override
        {
            reflector.SetMarker("Transform");

            const bool positionLocked = !IsEditable(ComponentType::Position);
            if (positionLocked) reflector.SetReadOnlyFlag(true);
            auto position = m_pos;
            if (reflector.Visit("Position", position)) SetPosition(position);
            if (positionLocked) reflector.SetReadOnlyFlag(false);

            const bool orientationLocked = !IsEditable(ComponentType::Orientation);
            if (orientationLocked) reflector.SetReadOnlyFlag(true);
            glm::vec3 euler = Quaternion::GetEulerAngles(m_orient);
            if (reflector.Visit("Orient", euler)) SetAngles(euler);
            if (orientationLocked) reflector.SetReadOnlyFlag(false);

            const bool scaleLocked = !IsEditable(ComponentType::Scale);
            if (scaleLocked) reflector.SetReadOnlyFlag(true);
            auto scale = m_scale;
            if (reflector.Visit("Scale", scale)) SetScale(scale);
            if (scaleLocked) reflector.SetReadOnlyFlag(false);
        }

        VariantData Serialize() const override
        {
            auto data = Base::Serialize();
            data["position"] = m_pos;
            data["euler"] = Quaternion::GetEulerAngles(m_orient);
            data["scale"] = m_scale;
            if (m_hasAffine)
            {
                auto& affine = data["affine"] = VariantData::Array();
                for (glm::length_t column = 0; column < 4; ++column) affine.PushBack(m_affine[column]);
            }
            return data;
        }

        MAYBE_UNUSED bool Deserialize(const VariantData& data) override
        {
            if (!Base::Deserialize(data)) return false;
            if (data.HasKey("affine"))
            {
                const auto& affine = data["affine"];
                if (!affine.IsArray() || affine.GetArray().size() != 4)
                {
                    LOG_ERROR(LogSerialize, "Transform affine matrix must contain four columns.");
                    return false;
                }
                glm::mat4x3 matrix(1.0f);
                for (glm::length_t column = 0; column < 4; ++column)
                {
                    const auto& value = affine.GetArray()[column];
                    if (!value.IsArray() || value.GetArray().size() != 3)
                    {
                        LOG_ERROR(LogSerialize, "Transform affine matrix columns must contain three values.");
                        return false;
                    }
                    matrix[column] = value.GetVec3();
                }
                SetAffine(matrix);
                return true;
            }
            SetPosition(data["position"].GetVec3());
            SetAngles(data["euler"].GetVec3());
            SetScale(data["scale"].GetVec3());
            return true;
        }

        bool IsEditable(ComponentType type) const
        {
            return !m_locks[INT_CAST(type)];
        }

        void SetPosition(glm::vec3 pos)
        {
            if (!IsEditable(ComponentType::Position)) return;
            m_pos = pos;
            if (m_hasAffine) m_affine[3] = pos;
            onTransformDirty(*this, ComponentType::Position);
        }

        const glm::vec3& GetPosition() const { return m_pos; }

        void Translate(const glm::vec3& translation)
        {
            if (!IsEditable(ComponentType::Position)) return;
            m_pos += translation;
            if (m_hasAffine) m_affine[3] = m_pos;
            onTransformDirty(*this, ComponentType::Position);
        }

        const glm::quat& GetQuaternion() const { return m_orient; }

        void SetQuaternion(const glm::quat& quat)
        {
            if (!IsEditable(ComponentType::Orientation)) return;
            m_orient = quat;
            m_hasAffine = false;
            onTransformDirty(*this, ComponentType::Orientation);
        }

        glm::vec3 GetAngles() const
        {
            return Quaternion::GetEulerAngles(m_orient);
        }

        void SetAngles(const glm::vec3& angles)
        {
            if (!IsEditable(ComponentType::Orientation)) return;
            m_orient = Quaternion::FromEulerAngles(angles);
            m_hasAffine = false;
            onTransformDirty(*this, ComponentType::Orientation);
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
            if (!IsEditable(ComponentType::Orientation)) return;
            // Checks for a zero vector.
            if (glm::all(glm::epsilonEqual(axis, glm::vec3(0), glm::vec3(FLT_EPSILON)))) return;

            m_orient = glm::rotate(m_orient, glm::radians(degree), axis);
            m_hasAffine = false;
            onTransformDirty(*this, ComponentType::Orientation);
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
            if (!IsEditable(ComponentType::Orientation)) return;
            m_orient *= glm::quat(glm::radians(eulerAngles));
            m_hasAffine = false;
            onTransformDirty(*this, ComponentType::Orientation);
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
            if (!IsEditable(ComponentType::Orientation)) return;
            m_orient = glm::quatLookAt(glm::normalize(center - pos), up);
            m_hasAffine = false;
            onTransformDirty(*this, ComponentType::Orientation);
        }

        NODISCARD const glm::vec3& GetScale() const { return m_scale; }

        void SetScale(const glm::vec3& scale)
        {
            if (!IsEditable(ComponentType::Scale)) return;
            m_scale = scale;
            m_hasAffine = false;
            onTransformDirty(*this, ComponentType::Scale);
        }

        void ApplyScale(const glm::vec3& scale)
        {
            if (!IsEditable(ComponentType::Scale)) return;
            m_scale *= scale;
            m_hasAffine = false;
            onTransformDirty(*this, ComponentType::Scale);
        }

        //! Preserves the full affine matrix, including shear. Singular matrices have
        //! no unique orientation; their TRS view uses identity rotation and column lengths.
        //! Editing rotation or scale replaces shear with the decomposed TRS transform.
        void SetAffine(const glm::mat4x3& affine)
        {
            m_affine = affine;
            m_hasAffine = true;
            glm::vec3 skew;
            glm::vec4 perspective;
            if (!glm::decompose(Matrix::ExpandAffine(affine), m_scale, m_orient, m_pos, skew, perspective))
            {
                m_pos = affine[3];
                m_scale = {glm::length(affine[0]), glm::length(affine[1]), glm::length(affine[2])};
                m_orient = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
            }
            onTransformDirty(*this, ComponentType::Position);
            onTransformDirty(*this, ComponentType::Orientation);
            onTransformDirty(*this, ComponentType::Scale);
        }

        NODISCARD glm::mat4x3 GetAffine() const
        {
            if (m_hasAffine) return m_affine;
            glm::mat4x3 mtx {1.0};
            const auto rotation = glm::mat3_cast(m_orient);
            const auto scale = m_scale;
            mtx[0] = rotation[0] * scale.x;
            mtx[1] = rotation[1] * scale.y;
            mtx[2] = rotation[2] * scale.z;
            mtx[3] = GetPosition();
            return mtx;
        }

        NODISCARD glm::mat4 GetMatrix() const
        {
            return Matrix::ExpandAffine(GetAffine());
        }

        sigslot::signal<const TransformComponent&, ComponentType> onTransformDirty;

    private:
        glm::vec3 m_pos {0};
        glm::vec3 m_scale {1.0f};
        glm::quat m_orient {1.0f, 0.0f, 0.0f, 0.0f};
        glm::bvec3 m_locks { false };
        glm::mat4x3 m_affine {1.0f};
        bool m_hasAffine = false;
    };
}