#pragma once

#include "EditorWidget.h"
#include "nexus/scene/Scene.h"

#include "entt/entt.hpp"

NXS_NAMESPACE
{
    class SceneGraphWidget : public EditorWidget
    {
    public:
        SceneGraphWidget(Ref<Scene> scene)
            : EditorWidget("Scene Graph")
            , m_scene(scene)
        {}

        ~SceneGraphWidget();

    protected:
        void Draw_Internal(RenderSystem& renderSystem) override;
        void DrawSceneNode(Ref<SceneNode> node);

    private:
        void DeleteNode(Ref<SceneNode> node);

        Ref<Scene> m_scene;
        Ref<SceneNode> m_highlightNode;
        Ref<SceneNode> m_selectedNode;
    };
}