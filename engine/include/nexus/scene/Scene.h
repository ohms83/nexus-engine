#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/ecs/Ecs.h"
#include "nexus/graphics/Color.h"
#include "nexus/core/LogDispatcher.h"
#include "nexus/core/Reflection.h"

#include "SceneNode.h"
#include "SceneRenderer.h"
#include "component/LightComponent.h"

DECLARE_LOG_EXTERN(Scene);

NXS_NAMESPACE
{
    class RenderSystem;
    class TaskScheduler;
    
    // TODO: Create a new parent class named Node which both Scene and SceneNode inherit from.
    class Scene : public IReflection
    {
    public:
        IMPLEMENT_REFLECTION(Scene);

        Scene();
        explicit Scene(const std::string& name);
        virtual ~Scene();

        void AcceptReflector(IReflector& reflector);

        NODISCARD const std::string& GetName() const { return m_name; }
        void ChangeName(const std::string& name) { m_name = name; }

        void SetTaskScheduler(Ref<TaskScheduler> taskScheduler) { m_scheduler = taskScheduler; }

        template<typename T>
        requires std::derived_from<T, SceneNode>
        MAYBE_UNUSED Ref<T> CreateNode(const std::string& name, const Ref<SceneNode>& parent = nullptr)
        {
            auto node = m_children.emplace_back(std::make_shared<T>(GetRegistry(), name));
            if (parent) {
                parent->AddChild(node);
            }
            return PTR_CAST<T>(node);
        }

        template<typename T>
        requires std::derived_from<T, SceneNode>
        MAYBE_UNUSED Ref<T> CreateNode(const std::string& name, const std::string& parentName)
        {
            const auto itr = std::ranges::find_if(m_children, [parentName](const Ref<SceneNode>& node) {
                return node->GetName() == parentName;
            });

            Ref<SceneNode> parent;
            if (itr == m_children.end()) {
                LOG_WARNING(LogScene, std::format("Cannot find a parent node {}", parentName));
            }
            else {
                parent = *itr;
            }
            return CreateNode<T>(name, parent);
        }
        
        NODISCARD Ref<SceneNode> FindNode(SceneNode::Id id);

        /**
         * Find a node with the specified @c name.
         * @param name Node name
         * @return A scene node with specified name or a nullptr, if not found
         */
        NODISCARD Ref<SceneNode> FindNodeWithName(const std::string& name);

        const SceneNode::ChildList& GetAllChildren() const
        {
            return m_children;
        }

        void GetAllRootNodes(SceneNode::ChildList& nodeList) const;

        void RemoveNode(Ref<SceneNode> node);
        void RemoveNodeByName(const std::string& name);

        virtual void OnEnter() {}
        virtual void OnExit() {}
        virtual void Update(float dt);
        virtual void Render(RenderSystem& renderSystem);

        NODISCARD entt::registry& GetRegistry()
        {
            return m_registry;
        }

        void SetRenderer(Ptr<ISceneRenderer> renderer);

        uint32_t AddSimulation(ECS::SimulationSystem system);
        void RemoveSimulation(uint32_t id);

        Color3F& Ambient();
        NODISCARD const Color3F& Ambient() const;

    private:
        void Init();

    protected:
        entt::entity m_entity;

        std::string m_name;
        std::vector<Ref<SceneNode>> m_children;
        Ref<TaskScheduler> m_scheduler;

        // --- Rendering ---
        Ptr<ISceneRenderer> m_renderer;

        // --- ECS ---
        struct Simulation
        {
            uint32_t id;
            ECS::SimulationSystem system;
        };

        entt::registry m_registry;
        std::vector<Simulation> m_simulations;
    };
}
