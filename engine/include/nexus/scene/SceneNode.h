#pragma once

#include "nexus/NxsDefine.h"

#include "Entity.h"
#include "Transform.h"

#include "nexus/ecs/component/scene/SceneNodeComponent.h"
#include "nexus/ecs/component/scene/TransformComponent.h"

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

        void AddChild(Ref<SceneNode> child)
        {
            m_children.push_back(child);
        }

        virtual void Update(float dt) {}

    protected:
        virtual void OnActivate() {};
        virtual void OnDeactivate() {};

        SceneNodeComponent& m_node;
        std::vector<Ref<SceneNode>> m_children;
    };
}