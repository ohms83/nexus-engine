#pragma once

#include "EditorWidget.h"
#include "nexus/scene/SceneNode.h"
#include "nexus/core/Reflection.h"

NXS_NAMESPACE
{
    class SceneManager;
    class Scene;

    class PropertyWindow : public EditorWidget, public IReflector
    {
    public:
        explicit PropertyWindow(SceneManager& sceneManager);
        virtual ~PropertyWindow() = default;

        void SetSceneNode(SceneNode::Id sceneNodeId);

        void Draw(RenderSystem& renderSystem);
        virtual void Update() {}
        
        void ChangeCatetory(const std::string& name) override;
        void VisitPropertyWithFeedback(const std::string& name, std::type_index type, void* value, std::function<void(void*)> callback) override;

    protected:
        void Draw_Internal(RenderSystem& renderSystem) override;

    private:
        SceneNode::Id m_sceneNodeId = SceneNode::InvalidID;
        Ref<Scene> m_currentScene;
    };
}