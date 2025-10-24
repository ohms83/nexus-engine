#pragma once

#include "EditorWidget.h"
#include "nexus/scene/Scene.h"

#include "entt/entt.hpp"

NXS_NAMESPACE
{
    class SceneGraphWidget : public EditorWidget
    {
    public:
        // TODO: Change parameter to SceneManager.
        SceneGraphWidget(Ref<Scene> scene)
            : EditorWidget("Scene Graph")
            , m_scene(scene)
        {}

        ~SceneGraphWidget();

        SceneNode::Id GetSelectedNode() const;

    protected:
        void Draw_Internal(RenderSystem& renderSystem) override;
        void DrawSceneNode(Ref<SceneNode> node);

    private:
        void DeleteNode(Ref<SceneNode> node);

        Ref<Scene> m_scene;
        // TODO: Use SceneNode::Id
        Ref<SceneNode> m_highlightNode;
        // TODO: Use SceneNode::Id
        Ref<SceneNode> m_selectedNode;
    };
}