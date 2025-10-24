#pragma once

#include "nexus/NxsDefine.h"

#include "Entity.h"
#include "Transform.h"
#include "Script.h"

#include "component/SceneNodeComponent.h"

#include <string>

NXS_NAMESPACE
{
    class Scene;

    class SceneNode : public Entity
    {
    public:
        using ChildList = std::vector<Ref<SceneNode>>;
        using Id = uint64_t;

        SceneNode() = delete;
        explicit SceneNode(entt::registry& registry);
        explicit SceneNode(entt::registry& registry, std::string  name);
        virtual ~SceneNode();

        void Destroy();

        NODISCARD Id GetId() const { return m_id; }

        NODISCARD const std::string& GetName() const
        {
            return GetComponent<SceneNodeComponent>().name;
        }

        void SetName(const std::string& name)
        {
            GetComponent<SceneNodeComponent>().name = name;
        }

        void Activate(bool activate);
        NODISCARD bool IsActive() const
        {
            return GetComponent<SceneNodeComponent>().active;
        }

        void AddChild(Ref<SceneNode> child);
        void RemoveChild(Ref<SceneNode> child);
        void GetAllChildren(ChildList& childrenList) const;
        void GetAllDescendants(ChildList& childrenList, bool parentFirst) const;

        bool HasChild() const { return m_children.size() > 0; }
        size_t GetNumChildren() const { return m_children.size(); }

        SceneNode* GetParent() const { return m_parent; }
        void RemoveFromParent();

        void AddScript(Ref<Script> script);
        void RemoveScript(Ref<Script> script);

        void Update(float dt);

        virtual void OnActivate() {};
        virtual void OnDeactivate() {};
        virtual void OnDestroy() {};
        virtual void OnUpdate(float dt) {}

        static const Id InvalidID;

    protected:
        Id m_id = InvalidID;
        Scene* m_owner = nullptr;
        SceneNode* m_parent = nullptr;
        ChildList m_children;
        std::vector<Ref<Script>> m_scripts;
    };
}