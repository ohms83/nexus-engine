#pragma once

#include "nexus/NxsDefine.h"

#include "Entity.h"
#include "Transform.h"
#include "Script.h"

#include "nexus/ecs/component/scene/SceneNodeComponent.h"

#include <string>

NXS_NAMESPACE
{
    class Scene;

    class SceneNode : public Entity
    {
    public:
        using ChildList = std::vector<Ref<SceneNode>>;

        SceneNode() = delete;
        explicit SceneNode(entt::registry& registry);
        explicit SceneNode(entt::registry& registry, std::string  name);
        virtual ~SceneNode() = default;

        void Destroy();

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

        void AddChild(Ref<SceneNode> child);
        void RemoveChild(Ref<SceneNode> child);
        void GetAllChildren(ChildList& childrenList) const;
        void GetAllDescendants(ChildList& childrenList, bool parentFirst) const;

        SceneNode* GetParent() const { return m_parent; }
        void RemoveFromParent();

        void AddScript(Ref<Script> script);
        void RemoveScript(Ref<Script> script);

        void Update(float dt);

        virtual void OnActivate() {};
        virtual void OnDeactivate() {};
        virtual void OnDestroy() {};
        virtual void OnUpdate(float dt) {}

    protected:
        Scene* m_owner;
        SceneNodeComponent& m_node;
        SceneNode* m_parent = nullptr;
        ChildList m_children;
        std::vector<Ref<Script>> m_scripts;
    };
}