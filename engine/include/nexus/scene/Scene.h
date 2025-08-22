#pragma once

#include "nexus/NxsDefine.h"
#include "SceneNode.h"
#include "SceneRenderer.h"
#include "nexus/ecs/Ecs.h"
#include "nexus/ecs/component/scene/LightComponent.h"
#include "nexus/graphics/Color.h"
#include "nexus/core/LogDispatcher.h"

DECLARE_LOG_EXTERN(Scene);

NXS_NAMESPACE
{
    class RenderSystem;

    class Scene
    {
    public:
        Scene();
        virtual ~Scene() = default;

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

        /**
         * Find a node with the specified @c name.
         * @param name Node name
         * @return A scene node with specified name or a nullptr, if not found
         */
        NODISCARD Ref<SceneNode> GetNode(const std::string& name);

        virtual void Update(float dt);
        virtual void Render(RenderSystem& renderSystem);

        NODISCARD entt::registry& GetRegistry()
        {
            return m_registry;
        }

        void SetRenderer(Ptr<ISceneRenderer> renderer);

        Color3F& Ambient();
        const Color3F& Ambient() const;

    protected:
        std::vector<Ref<SceneNode>> m_children;

        // --- Rendering ---
        //! Ambient light.
        AmbientLightComponent* m_ambientComponent = nullptr;
        Ptr<ISceneRenderer> m_renderer;

        // --- ECS ---
        entt::registry m_registry;
        std::vector<ECS::SimulationSystem> m_simulations;
    };
}
