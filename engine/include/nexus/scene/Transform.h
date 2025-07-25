//
// Created by nutta on 7/8/2025.
//

#pragma once

#include <vector>
#include <nexus/NxsCommon.h>

NXS_NAMESPACE
{
    class Transform final
    {
    public:
        enum class Space
        {
            Local,
            Global,
            Num
        };

        void Translate(const glm::vec3& distance)
        {
            m_position += distance;
            m_needUpdate = true;
        }
        void SetPosition(const glm::vec3& position)
        {
            m_position = position;
            m_needUpdate = true;
        }
        NODISCARD const glm::vec3& GetPosition(Space space = Space::Local) const;

        void Rotate(const float degree, const glm::vec3& axis)
        {
            m_orientation = glm::rotate(m_orientation, glm::radians(degree), axis);
            m_needUpdate = true;
        }
        void SetOrientation(const glm::quat& orientation)
        {
            m_orientation = orientation;
            m_needUpdate = true;
        }
        NODISCARD const glm::quat& GetOrient(Space space = Space::Local) const;

        void Scale(const glm::vec3& scale)
        {
            m_scale *= scale;
            m_needUpdate = true;
        }
        NODISCARD const glm::vec3& GetScale(Space space = Space::Local) const;

        // TODO: Should consider using a matrix 4x3 instead, since the homogenous coordinate barely relevant.
        /**
         * Compute a 4x4 transformation matrix of this Transform object.
         * @param transformSpace Local space will return the transformation matrix relative to its local coordinate,
         * while Global will base the transformation on its parent and predecessors.
         * @return A 4x4 transformation matrix.
         */
        NODISCARD glm::mat4 GetMatrix(Space transformSpace = Space::Local) const;

        void AddChild(Transform* child);
        void RemoveChild(Transform* child);
        NODISCARD Transform* GetParent() const
        {
            return m_parent;
        }
        NODISCARD bool HasParent() const
        {
            return m_parent != nullptr;
        }

        /**
         * Orienting this transform to make its forward vector pointing at the target.
         * @param target The target position.
         * @param up The up vector telling how this transform is oriented. Typically (0, 1, 0).
         */
        void LookAt(const glm::vec3& target, const glm::vec3& up);

        //! Return transform's right vector
        NODISCARD glm::vec3 Right() const;
        //! Return transform's up vector
        NODISCARD glm::vec3 Up() const;
        //! Return transform's forward vector
        NODISCARD glm::vec3 Forward() const;

    private:
        Transform* m_parent = nullptr;
        std::vector<Transform*> m_children;

        glm::vec3 m_position{};
        glm::quat m_orientation{ 1, 0, 0, 0 };
        glm::vec3 m_scale{1, 1, 1};

        glm::mat4x3 m_transformMtx{1.0f};

        bool m_needUpdate = false;
    };
}
