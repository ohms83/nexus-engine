#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/core/serialize/Serializeable.h"
#include "nexus/core/Reflection.h"
#include "nexus/ecs/Ecs.h"
#include "nexus/core/task/TaskScheduler.h"
#include "nexus/graphics/RenderingInterface.h"

#include "Entity.h"
#include "Transform.h"
#include "Script.h"
#include "component/SceneNodeComponent.h"

#include <string>
#include <memory>

NXS_NAMESPACE
{
    class Scene;

    class SceneNode : public Entity, public IReflection, public ISerializeable
    // TODO: Consider replacing std::enable_shared_from_this with a custom solution
    , public std::enable_shared_from_this<SceneNode>
    {
    public:
        using ChildList = std::vector<Ref<SceneNode>>;

        IMPLEMENT_REFLECTION(SceneNode);

        SceneNode() = delete;
        explicit SceneNode(Ref<entt::registry> registry, std::string  name = "");
        virtual ~SceneNode();

        void AcceptReflector(IReflector& reflector) override;

        VariantData Serialize() const override;
        void Deserialize(const VariantData& data) override;

        /**
         * @brief Resolve object's dependencies. This must be called after `Deserialization`.
         * 
         * @param renderingInterface 
         */
        virtual void Resolve(RenderingInterface& renderingInterface);

        Ref<SceneNode> GetSelf() { return PTR_CAST<SceneNode>(shared_from_this()); }

        void Destroy();

        NODISCARD Identifier GetId() const { return GetComponent<SceneNodeComponent>()->id; }

        NODISCARD const std::string& GetName() const
        {
            return GetComponent<SceneNodeComponent>()->name;
        }

        void SetName(const std::string& name)
        {
            GetComponent<SceneNodeComponent>()->name = name;
        }

        void Activate(bool activate);
        NODISCARD bool IsActive() const
        {
            return GetComponent<SceneNodeComponent>()->active;
        }

        template<typename T, typename... Args>
        requires std::derived_from<T, SceneNode>
        MAYBE_UNUSED Ref<T> EmplaceChild(Args&&... args)
        {
            auto node = m_children.emplace_back(
                std::make_shared<T>(GetRegistry(), std::forward<Args>(args)...));
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

        NODISCARD Ref<SceneNode> FindNode(Identifier id);

        /**
         * Find a node with the specified @c name.
         * @param name Node name
         * @return A scene node with specified name or a nullptr, if not found
         */
        NODISCARD Ref<SceneNode> FindNodeWithName(const std::string& name);

        bool HasChild() const { return m_children.size() > 0; }
        size_t GetNumChildren() const { return m_children.size(); }

        Ref<SceneNode> GetParent() const { return m_parent; }
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

    protected:
        Ref<SceneNode> m_parent;
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