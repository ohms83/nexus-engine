#pragma once

#include "EditorWidget.h"
#include "nexus/scene/Scene.h"

#include "entt/entt.hpp"

NXS_NAMESPACE
{
    class SceneGraphWidget : public EditorWidget
    {
    public:
        SceneGraphWidget(Ref<const Scene> scene)
            : EditorWidget("Scene Graph")
            , m_scene(scene)
        {}

    protected:
        void Draw_Internal(RenderSystem& renderSystem) override;
        void DrawSceneNode(Ref<const SceneNode> node);

    private:
        Ref<const Scene> m_scene;
        Ref<const SceneNode> m_selectedNode;
    };
}