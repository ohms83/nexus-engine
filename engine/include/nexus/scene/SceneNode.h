#pragma once

#include "nexus/NxsDefine.h"
#include "Transform.h"

#include <string>

NXS_NAMESPACE
{
    class SceneNode
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
            return m_name;
        }

        void SetName(const std::string& name)
        {
            m_name = name;
        }

        void Activate(bool activate);
        NODISCARD bool IsActive() const
        {
            return m_active;
        }

        virtual void Update(float dt) {}

        entt::registry& GetRegistry() const
        {
            return *m_registry;
        }

        template<typename Type, typename... Args>
        void AddComponent(Args &&...args)
        {
            m_registry->emplace<Type>(m_entity, std::forward<Args>(args)...);
        }

        template<typename... Type>
        NODISCARD decltype(auto) GetComponent() const
        {
            return m_registry->get<Type...>(m_entity);
        }

        virtual void Translate(const glm::vec3& translation);
        virtual void SetPosition(const glm::vec3& position);
        virtual const glm::vec3& GetPosition() const;

        virtual void Rotate(float degree, const glm::vec3& axis);
        virtual void SetRotation(const glm::quat& rotation);
        virtual const glm::quat& GetRotation() const;

        virtual void Scale(const glm::vec3& scale);
        virtual void SetScale(const glm::vec3& scale);
        virtual const glm::vec3& GetScale() const;

        void LookAt(const glm::vec3& center, const glm::vec3& up);

        Transform transform;

    protected:
        virtual void OnActivate() {};
        virtual void OnDeactivate() {};

        std::string m_name;
        bool m_active = true;
        entt::entity m_entity{};

    private:
        entt::registry* m_registry = nullptr;
    };
}