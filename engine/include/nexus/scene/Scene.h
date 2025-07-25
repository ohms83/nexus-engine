#pragma once

#include "nexus/NxsDefine.h"
#include "SceneNode.h"
#include "SceneRenderer.h"
#include "core/Logger.h"
#include "graphics/Color.h"

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
        Ref<T> CreateNode(const std::string& name, const Ref<SceneNode>& parent = nullptr)
        {
            auto& node = m_children.emplace_back(std::make_shared<T>(GetRegistry(), name));
            if (parent) {
                parent->transform.AddChild(&node->transform);
            }
            return PTR_CAST<T>(node);
        }

        template<typename T>
        requires std::derived_from<T, SceneNode>
        Ref<T> CreateNode(const std::string& name, const std::string& parentName)
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

        virtual void Update();
        virtual void Render(RenderSystem& renderSystem);

        entt::registry& GetRegistry()
        {
            return m_registry;
        }

        void SetRenderer(Ptr<ISceneRenderer> renderer);

        void SetAmbient(const Color3F& color);

    protected:
        entt::registry m_registry;
        //! Ambient light.
        entt::entity m_ambient;
        std::vector<Ref<SceneNode>> m_children;
        Ptr<ISceneRenderer> m_renderer;
    };
}
