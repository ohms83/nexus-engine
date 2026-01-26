#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/core/serialize/Serializeable.h"
#include "nexus/core/resource/ResourceManager.h"
#include "nexus/core/task/TaskScheduler.h"
#include "nexus/core/Reflection.h"
#include "nexus/ecs/Ecs.h"
#include "nexus/ecs/Entity.h"
#include "nexus/graphics/RenderingInterface.h"

#include "Transform.h"
#include "Script.h"
#include "component/SceneNodeComponent.h"

#include <string>
#include <memory>
#include <unordered_map>

#define IMPLEMENT_NODE(Type) \
    IMPLEMENT_REFLECTION(Type); \
    static void Register() { \
        SceneNode::s_factoryFunctions[#Type] = [](Ref<entt::registry> registry) { \
            return std::make_shared<Type>(registry, "New " #Type); \
        }; \
    }

NXS_NAMESPACE
{
    class SceneNode : public Entity, public IReflection
    // TODO: Consider replacing std::enable_shared_from_this with a custom solution
    , public std::enable_shared_from_this<SceneNode>
    {
    public:
        using ChildList = std::vector<Ref<SceneNode>>;
        using Creator = std::function<Ref<SceneNode>(Ref<entt::registry>)>;

        IMPLEMENT_NODE(SceneNode);

        SceneNode() = delete;
        explicit SceneNode(Ref<entt::registry> registry, std::string  name = "");
        virtual ~SceneNode();

        static Ref<SceneNode> Create(Ref<entt::registry> registry, std::string className);
        static Ref<SceneNode> CreateChild(Ref<SceneNode> parent, std::string className);
        static void GetRegisteredNodeTypes(std::vector<std::string>& outTypes);

        void AcceptReflector(IReflector& reflector) override;

        VariantData Serialize() const override;
        MAYBE_UNUSED bool Deserialize(const VariantData& data) override;

        /**
         * @brief Resolve object's dependencies. This must be called after `Deserialization`.
         * 
         * @param resourceManager The resource manager used to resolve resources.
         */
        virtual void Resolve(IResourceManager& resourceManager);

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
            node->m_parent = GetSelf();
            node->SetTaskScheduler(m_scheduler);
            return PTR_CAST<T>(node);
        }

        void AddChild(Ref<SceneNode> child);
        void RemoveChild(Ref<SceneNode> child);
        void RemoveAllChildren();
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

    private:
        /**
         * @brief Validate and clamp component properties. This should be performed
         * after deserialization to ensure data integrity (eg. divide by zero prevention).
         * @note This function is optional to implement in derived classes.
         */
        virtual void Validate();

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

        static std::unordered_map<std::string, Creator> s_factoryFunctions;
    };
}