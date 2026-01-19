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

        void SetSceneNode(Identifier sceneNodeId);

        virtual void Update() override {}
        
        void ChangeCategory(const std::string& name) override;
        void VisitReadOnlyProperty(const std::string& name, std::type_index type, void* value) override;
        void VisitPropertyWithFeedback(const std::string& name, std::type_index type, void* value, std::function<void(void*)> callback) override;

    protected:
        void Draw_Internal(RenderSystem& renderSystem) override;
        void ChangeCurrentScene(Ref<Scene> scene);

    private:
        Identifier m_sceneNodeId = InvalidID;
        Ref<Scene> m_currentScene;
        uint32_t m_currentCategory = 0;
    };
}