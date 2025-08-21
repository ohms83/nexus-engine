#pragma once

#include "nexus/NxsDefine.h"

#include "Entity.h"
#include "Transform.h"

#include "nexus/ecs/component/scene/SceneNodeComponent.h"

#include <string>

NXS_NAMESPACE
{
    class SceneNode : public Entity
    {
    public:
        SceneNode() = delete;
        explicit SceneNode(entt::registry& registry);
        explicit SceneNode(entt::registry& registry, std::string  name);
        virtual ~SceneNode() = default;

        NODISCARD glm::mat4 GetMatrix() const
        {
            return transform.GetMatrix();
        }

        NODISCARD const std::string& GetName() const
        {
            return m_node.name;
        }

        void SetName(const std::string& name)
        {
            m_node.name = name;
        }

        void Activate(bool activate);
        NODISCARD bool IsActive() const
        {
            return m_node.active;
        }

        virtual void Update(float dt) {}

        virtual void Translate(const glm::vec3& translation);
        virtual void SetPosition(const glm::vec3& position);
        NODISCARD virtual const glm::vec3& GetPosition() const;

        virtual void Rotate(float degree, const glm::vec3& axis);
        //! Rotating the node
        virtual void Rotate(const glm::vec3& eulerAngles);
        virtual void SetRotation(const glm::quat& rotation);
        NODISCARD virtual const glm::quat& GetRotation() const;

        virtual void Scale(const glm::vec3& scale);
        virtual void SetScale(const glm::vec3& scale);
        NODISCARD virtual const glm::vec3& GetScale() const;

        void LookAt(const glm::vec3& center, const glm::vec3& up);

        Transform transform;

    protected:
        virtual void OnActivate() {};
        virtual void OnDeactivate() {};

        SceneNodeComponent& m_node;
    };
}