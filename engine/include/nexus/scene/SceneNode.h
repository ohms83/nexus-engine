#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/core/Reflection.h"
#include "nexus/ecs/Ecs.h"
#include "nexus/core/task/TaskScheduler.h"

#include "Entity.h"
#include "Transform.h"
#include "Script.h"

#include "component/SceneNodeComponent.h"

#include <string>

NXS_NAMESPACE
{
    class Scene;

    class SceneNode : public Entity, public IReflection
    {
    public:
        using ChildList = std::vector<Ref<SceneNode>>;
        using Id = uint64_t;

        IMPLEMENT_REFLECTION(SceneNode);

        SceneNode() = delete;
        explicit SceneNode(Ref<entt::registry> registry);
        explicit SceneNode(Ref<entt::registry> registry, std::string  name);
        virtual ~SceneNode();

        void AcceptReflector(IReflector& reflector) override;

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

        template<typename T>
        requires std::derived_from<T, SceneNode>
        MAYBE_UNUSED Ref<T> EmplaceChild(const std::string& name)
        {
            auto node = m_children.emplace_back(std::make_shared<T>(GetRegistry(), name));
            return PTR_CAST<T>(node);
        }

        void AddChild(Ref<SceneNode> child);
        void RemoveChild(Ref<SceneNode> child);
        /**
         * @brief Get all direct child nodes.
         * 
         * @param childrenList 
         */
        void GetAllChildren(ChildList& childrenList) const;
        /**
         * @brief Get all descendant nodes; including children of children.
         * 
         * @param childrenList 
         */
        void GetAllDescendants(ChildList& childrenList, bool parentFirst) const;

        NODISCARD Ref<SceneNode> FindNode(SceneNode::Id id);

        /**
         * Find a node with the specified @c name.
         * @param name Node name
         * @return A scene node with specified name or a nullptr, if not found
         */
        NODISCARD Ref<SceneNode> FindNodeWithName(const std::string& name);

        bool HasChild() const { return m_children.size() > 0; }
        size_t GetNumChildren() const { return m_children.size(); }

        SceneNode* GetParent() const { return m_parent; }
        void RemoveFromParent();

        void SetTaskScheduler(Ref<TaskScheduler> taskScheduler) { m_scheduler = taskScheduler; }

        void AddScript(Ref<Script> script);
        void RemoveScript(Ref<Script> script);

        uint32_t AddSimulation(ECS::SimulationSystem system);
        void RemoveSimulation(uint32_t id);

        void Update(float dt);

        virtual void OnActivate() {};
        virtual void OnDeactivate() {};
        virtual void OnDestroy() {};
        virtual void OnUpdate(float dt) {}

        static const Id InvalidID;

    protected:
        Id m_id = InvalidID;
        SceneNode* m_parent = nullptr;
        ChildList m_children;
        Ref<TaskScheduler> m_scheduler;
        std::vector<Ref<Script>> m_scripts;

        // --- ECS ---
        struct Simulation
        {
            uint32_t id;
            ECS::SimulationSystem system;
        };
        std::vector<Simulation> m_simulations;
    };
}