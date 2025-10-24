#pragma once

#include "EditorWidget.h"
#include "nexus/scene/Scene.h"
#include "nexus/scene/SceneManager.h"

#include "entt/entt.hpp"

NXS_NAMESPACE
{
    class SceneGraphWidget : public EditorWidget
    {
    public:
        // TODO: Change parameter to SceneManager.
        SceneGraphWidget(SceneManager& sceneManager);
        ~SceneGraphWidget();

        SceneNode::Id GetSelectedNode() const;

    protected:
        void Draw_Internal(RenderSystem& renderSystem) override;
        void DrawSceneNode(Ref<SceneNode> node);
        void HandleInput(Ref<SceneNode> node);
        void ShowContextMenu();

    private:
        void DeleteNode(Ref<SceneNode> node);

        Ref<Scene> m_scene;
        SceneNode::Id m_selectedNode;
    };
}