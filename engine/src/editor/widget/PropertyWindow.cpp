#include "editor/widget/PropertyWindow.h"
#include "scene/SceneManager.h"
#include "scene/component/TransformComponent.h"

#include "imgui.h"

USING_NAMESPACE_NXS;

PropertyWindow::PropertyWindow(SceneManager& sceneManager)
    : EditorWidget("Property Window")
    , m_sceneNodeId(SceneNode::InvalidID)
{
    sceneManager.sceneChangedCallback.connect([this](Ref<Scene> prev, Ref<Scene> current) {
        m_currentScene = current;
    });
    m_currentScene = sceneManager.GetCurrentScene();
}

void PropertyWindow::SetSceneNode(SceneNode::Id sceneNodeId)
{
    m_sceneNodeId = sceneNodeId;
}

void PropertyWindow::Draw_Internal(RenderSystem& renderSystem)
{
    if (m_sceneNodeId == SceneNode::InvalidID || !m_currentScene) return;

    auto selectedNode = m_currentScene->FindNode(m_sceneNodeId);
    if (!selectedNode)
    {
        m_sceneNodeId = SceneNode::InvalidID;
        return;
    }

    ImGui::SeparatorText("Properties");
    {
        auto comp = selectedNode->TryGetComponent<SceneNodeComponent>();
        if (comp)
        {
            char buf[2048] = "";
            const auto size = sizeof(buf);
            strncpy(buf, comp->name.c_str(), size);
            ImGui::InputText("Name", buf, size);
            ImGui::Checkbox("Active", &comp->active);

            if (strncmp(buf, comp->name.c_str(), size) != 0) comp->name = buf;
        }
    }

    ImGui::SeparatorText("Transform");

    {
        auto comp = selectedNode->TryGetComponent<PositionComponent>();
        if (comp)
        {
            ImGui::InputFloat3("Position", R_CAST<float*>(&comp->value));
        }
    }

    {
        auto comp = selectedNode->TryGetComponent<ScaleComponent>();
        if (comp)
        {
            ImGui::InputFloat3("Scale", R_CAST<float*>(&comp->value));
        }
    }
}