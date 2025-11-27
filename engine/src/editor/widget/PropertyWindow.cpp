#include "editor/widget/PropertyWindow.h"
#include "scene/SceneManager.h"
#include "math/MathUtil.h"
#include "core/Hasher.h"

#include "imgui.h"

#include "glm/gtx/euler_angles.hpp" // Required for eulerAngleXYZ

USING_NAMESPACE_NXS;

PropertyWindow::PropertyWindow(SceneManager& sceneManager)
    : EditorWidget("Property Window")
{
    sceneManager.sceneChangedCallback.connect([this](Ref<Scene> prev, Ref<Scene> current) {
        ChangeCurrentScene(current);
    });
    ChangeCurrentScene(sceneManager.GetCurrentScene());
}

void PropertyWindow::ChangeCurrentScene(Ref<Scene> scene)
{
    m_currentScene = scene;
    m_sceneNodeId = m_currentScene ? m_currentScene->GetId() : InvalidID;
}

void PropertyWindow::SetSceneNode(Identifier sceneNodeId)
{
    m_sceneNodeId = sceneNodeId;
}

void PropertyWindow::Draw_Internal(RenderSystem& renderSystem)
{
    if (m_sceneNodeId == InvalidID || !m_currentScene) return;

    auto selectedNode = m_currentScene->GetId() == m_sceneNodeId ? m_currentScene : m_currentScene->FindNode(m_sceneNodeId);
    if (!selectedNode)
    {
        m_sceneNodeId = InvalidID;
        return;
    }

    selectedNode->AcceptReflector(CAST<IReflector&>(*this));
}

void PropertyWindow::ChangeCategory(const std::string& name)
{
    if (const auto hash = Hasher().Hash32(name); hash != m_currentCategory)
    {
        ImGui::SeparatorText(name.c_str());
        m_currentCategory = hash;
    }
}

void PropertyWindow::VisitReadOnlyProperty(const std::string& name, std::type_index type, void* value)
{
    const char* const c_name = name.c_str();

    ImGui::BeginDisabled();

    if (type == typeid(std::string))
    {
        char buf[2048] = "";
        const char* const str = (const char* const)value;
        const auto size = sizeof(buf);
        strncpy(buf, str, size);

        ImGui::InputText(c_name, buf, size);
    }
    else if (type == typeid(int32_t))
    {
        static char buf[24] = "";
        snprintf(buf, sizeof(char) * 24, "%d", *(int32_t*)value);
        ImGui::LabelText(c_name, buf);
    }
    else if (type == typeid(bool))
    {
        bool flag = (bool*)value;
        ImGui::Checkbox(c_name, &flag);
    }
    else if (type == typeid(float))
    {
        ImGui::InputFloat(c_name, (float*)value, 0, 0, "%.6f");
    }
    else if (type == typeid(glm::vec2))
    {
        ImGui::InputFloat2(c_name, (float*)value, "%.3f");
    }
    else if (type == typeid(glm::vec3))
    {
        ImGui::InputFloat3(c_name, (float*)value, "%.3f");
    }
    else if (type == typeid(glm::vec4))
    {
        ImGui::InputFloat4(c_name, (float*)value, "%.3f");
    }
    else if (type == typeid(Color3F))
    {
        ImGui::ColorPicker3(c_name, (float*)value);
    }
    else if (type == typeid(Color4F))
    {
        ImGui::ColorPicker4(c_name, (float*)value);
    }

    ImGui::EndDisabled();
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
}