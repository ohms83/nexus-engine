#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/Container.h"
#include "nexus/serialize/Serializeable.h"
#include "nexus/resource/ResourceManager.h"
#include "nexus/reflection/Reflection.h"
#include "nexus/ecs/Ecs.h"
#include "nexus/ecs/Entity.h"
#include "nexus/graphics/RenderingInterface.h"

#include "Script.h"
#include "component/SceneNodeComponent.h"

#include <string>
#include <memory>
#include <unordered_map>

#include "Remotery.h"

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
    , public std::enable_shared_from_this<SceneNode>
    {
    public:
        using ChildList = Array<Ref<SceneNode>>;
        using Creator = std::function<Ref<SceneNode>(Ref<entt::registry>)>;

        IMPLEMENT_NODE(SceneNode);

        SceneNode() = delete;
        explicit SceneNode(Ref<entt::registry> registry, std::string  name = "");
        virtual ~SceneNode();

        static Ref<SceneNode> Create(Ref<entt::registry> registry, std::string className);
        static Ref<SceneNode> CreateChild(Ref<SceneNode> parent, std::string className);
        static void GetRegisteredNodeTypes(Array<std::string>& outTypes);

        void AcceptReflector(IReflector& reflector) override;

        VariantData Serialize() const override;
        MAYBE_UNUSED bool Deserialize(const VariantData& data) override;

        /**
         * @brief Resolve object's dependencies. This must be called after `Deserialization`.
         * 
         * @param resourceManager The resource manager used to resolve resources.
         * @param renderingInterface The rendering interface used to resolve graphical elements
         */
        virtual void Resolve(ResourceManager& resourceManager, const RenderingInterface& renderingInterface);
        /**
         * @brief Validate and clamp component properties. This should be performed
         * after deserialization to ensure data integrity (eg. divide by zero prevention).
         * @note This function is optional to implement in derived classes.
         */
        virtual void Validate();

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
            auto node = std::make_shared<T>(GetRegistry(), std::forward<Args>(args)...);
            AddChild(node);
            return PTR_CAST<T>(node);
        }

        void AddChild(Ref<SceneNode> child);
        void AddChildren(const ChildList& children);
        /**
         * @brief Remove a child node from this node.
         * If @c reparentDescendants is true, the children of the removed node will be reparented to this node.
         * 
         * @param child A reference to the child node to be removed.
         * @param reparentDescendants Whether to reparent the descendants of the removed child to this node. Default is true.
         */
        void RemoveChild(Ref<SceneNode> child, bool reparentDescendants = true);
        void RemoveAllChildren(bool reparentDescendants = true);
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

        template<typename T>
        NODISCARD Array<T*> GetAllDescendantsOfType(bool parentFirst) const
        {
            rmt_ScopedCPUSample(SceneNode_GetAllDescendantsOfType, 0)
            Array<T*> children;
            AppendAllDescendantsOfType<T>(children, parentFirst);
            return children;
        }

        template<typename TVisitor>
        void VisitDescendants(TVisitor&& visitor) const
        {
            rmt_ScopedCPUSample(SceneNode_VisitDescendants, 0)
            VisitDescendantsImpl(visitor);
        }

        NODISCARD Ref<SceneNode> FindChild(Identifier id, bool recursive = false);
        /**
         * Find a node with the specified @c id.
         * @param id Node ID
         * @return A scene node with specified ID or a nullptr, if not found
         */
        NODISCARD static SceneNode* FindNode(Identifier id);

        /**
         * Find a node with the specified @c name.
         * @param name Node name
         * @return A scene node with specified name or a nullptr, if not found
         */
        NODISCARD Ref<SceneNode> FindNodeWithName(const std::string& name);

        bool HasChild() const { return m_children.size() > 0; }
        size_t GetNumChildren() const { return m_children.size(); }

        Ref<SceneNode> GetParent() const { return m_parent.lock(); }
        void RemoveFromParent(bool reparentDescendants = true);

        void SetPosition(const glm::vec3& position);
        void SetOrientation(const glm::quat& orientation);
        void SetScale(const glm::vec3& scale);
        NODISCARD const glm::vec3& GetPosition() const;
        NODISCARD glm::quat GetOrientation() const;
        NODISCARD glm::vec3 GetScale() const;

        void SetLocalTransform(const glm::mat4& transform);
        //! Returns this node's transform relative to its parent.
        const glm::mat4& GetLocalTransform() const;
        void UpdateTransform() const;
        void UpdateGlobalTransform();
        //! Returns this node's transform composed with all its ancestors.
        NODISCARD glm::mat4 GetGlobalTransform() const;
        NODISCARD glm::vec3 GetGlobalPosition() const;
        NODISCARD glm::quat GetGlobalOrientation() const;
        NODISCARD glm::vec3 GetGlobalScale() const;

        void LookAt(const glm::vec3& center, const glm::vec3& up);

        NODISCARD glm::vec3 Right() const;
        NODISCARD glm::vec3 Up() const;
        NODISCARD glm::vec3 Forward() const;

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
        template<typename TVisitor>
        void VisitDescendantsImpl(TVisitor& visitor) const
        {
            for (const auto& child : m_children)
            {
                rmt_ScopedCPUSample(SceneNode_VisitDescendants_Visitor, 0)
                visitor(*child);
                child->VisitDescendantsImpl(visitor);
            }
        }

        template<typename T>
        void AppendAllDescendantsOfType(Array<T*>& children, bool parentFirst) const
        {
            rmt_ScopedCPUSample(SceneNode_AppendAllDescendantsOfType, 0)
            for (const auto& child : m_children)
            {
                auto tChild = D_CAST<T*>(child.get());
                if (parentFirst && tChild)
                {
                    children.push_back(tChild);
                }

                child->AppendAllDescendantsOfType<T>(children, parentFirst);

                if (!parentFirst && tChild)
                {
                    children.push_back(tChild);
                }
            }
        }

    protected:
        WeakRef<SceneNode> m_parent;
        ChildList m_children;
        Array<Ref<Script>> m_scripts;

        mutable glm::mat4 m_transform { 1.0f };
        mutable glm::mat4 m_globalTransform { 1.0f };
        mutable bool m_transformDirty = true;
        bool m_localTransformExplicit = false;

        // --- ECS ---
        struct Simulation
        {
            uint32_t id;
            ECS::SimulationSystem system;
        };
        Array<Simulation> m_simulations;

        static HashMap<std::string, Creator> s_factoryFunctions;
        static HashMap<Identifier, SceneNode*> s_nodeRegistry;
    };
}