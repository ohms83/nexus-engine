#pragma once

#include "nexus/NxsDefine.h"
#include "SceneNode.h"

NXS_NAMESPACE
{
    class RenderSystem;

    class Scene
    {
    public:
        Scene() = default;
        virtual ~Scene() = default;

        Ref<SceneNode> CreateNode(const std::string& name, Ref<SceneNode> parent = nullptr);
        Ref<SceneNode> CreateNode(const std::string& name, const std::string& parentName);

        virtual void Update();
        virtual void Render(RenderSystem& renderSystem);

    protected:
        std::vector<Ref<SceneNode>> m_children;
    };
}