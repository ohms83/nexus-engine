#pragma once

#include "EditorWidget.h"
#include "nexus/scene/SceneNode.h"

NXS_NAMESPACE
{
    class SceneManager;
    class Scene;

    class PropertyWindow : public EditorWidget
    {
    public:
        explicit PropertyWindow(SceneManager& sceneManager);
        virtual ~PropertyWindow() = default;

        void SetSceneNode(SceneNode::Id sceneNodeId);

        void Draw(RenderSystem& renderSystem);
        virtual void Update() {}

    protected:
        void Draw_Internal(RenderSystem& renderSystem) override;

    private:
        SceneNode::Id m_sceneNodeId = SceneNode::InvalidID;
        Ref<Scene> m_currentScene;
    };
}