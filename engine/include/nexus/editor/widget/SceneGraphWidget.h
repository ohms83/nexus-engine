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

        Identifier GetSelectedNode() const;

    protected:
        void Draw_Internal(RenderSystem& renderSystem) override;
        void SetCurrentScene(Ref<Scene> scene);
        void DrawSceneNode(Ref<SceneNode> node);
        void HandleInput(Ref<SceneNode> node);
        void ShowContextMenu();

    private:
        void DeleteNode(Ref<SceneNode> node);

        Ref<Scene> m_scene;
        Identifier m_selectedNode;
    };
}