#include "editor/widget/PropertyWindow.h"
#include "scene/SceneManager.h"
#include "scene/component/TransformComponent.h"
#include "math/MathUtil.h"

#include "imgui.h"

#include "glm/gtx/euler_angles.hpp" // Required for eulerAngleXYZ

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

    auto selectedNode = m_currentScene->GetId() == m_sceneNodeId ? m_currentScene : m_currentScene->FindNode(m_sceneNodeId);
    if (!selectedNode)
    {
        m_sceneNodeId = SceneNode::InvalidID;
        return;
    }

    selectedNode->AcceptReflector(CAST<IReflector&>(*this));
}

void PropertyWindow::ChangeCatetory(const std::string& name)
{
    ImGui::SeparatorText(name.c_str());
}

void PropertyWindow::VisitPropertyWithFeedback(const std::string& name, std::type_index type, void* value, std::function<void(void*)> callback)
{
    const char* const c_name = name.c_str();

    if (type == typeid(std::string))
    {
        char buf[2048] = "";
        const char* const str = (const char* const)value;
        const auto size = sizeof(buf);
        strncpy(buf, str, size);

        ImGui::InputText(c_name, buf, size);
        if (strncmp(buf, str, size) != 0) callback(buf);
    }
    else if (type == typeid(int32_t))
    {
        if (ImGui::InputInt(c_name, (int32_t*)value)) callback(value);
    }
    else if (type == typeid(bool))
    {
        if (ImGui::Checkbox(c_name, (bool*)value)) callback(value);
    }
    else if (type == typeid(float))
    {
        if (ImGui::InputFloat(c_name, (float*)value, 0, 0, "%.6f")) callback(value);
    }
    else if (type == typeid(glm::vec2))
    {
        if (ImGui::InputFloat2(c_name, (float*)value)) callback(value);
    }
    else if (type == typeid(glm::vec3))
    {
        if (ImGui::InputFloat3(c_name, (float*)value)) callback(value);
    }
    else if (type == typeid(glm::vec4))
    {
        if (ImGui::InputFloat4(c_name, (float*)value)) callback(value);
    }
    else if (type == typeid(Color3F))
    {
        if (ImGui::ColorPicker3(c_name, (float*)value)) callback(value);
    }
    else if (type == typeid(Color4F))
    {
        if (ImGui::ColorPicker4(c_name, (float*)value)) callback(value);
    }
    else if (type == typeid(OrientationComponent))
    {
        auto* comp = (OrientationComponent*)value;
        if (ImGui::InputFloat3(c_name, (float*)&comp->euler))
        {
            const auto radians = glm::radians(comp->euler);
            comp->quat = glm::quat(radians);
            callback(value);
        }
    }
}