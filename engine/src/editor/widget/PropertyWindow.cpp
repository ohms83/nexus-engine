#include "editor/widget/PropertyWindow.h"
#include "nexus/scene/SceneManager.h"
#include "nexus/math/MathUtil.h"
#include "nexus/core/Hasher.h"

#include "imgui.h"

#include <vector>

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
    m_sceneNodeId = m_currentScene ? m_currentScene->GetId() : NXS_INVALID_ID;
}

void PropertyWindow::SetSceneNode(Identifier sceneNodeId)
{
    m_sceneNodeId = sceneNodeId;
}

void PropertyWindow::Draw_Internal(RenderSystem& renderSystem)
{
    if (m_sceneNodeId == NXS_INVALID_ID || !m_currentScene) return;

    auto selectedNode = m_currentScene->GetId() == m_sceneNodeId ? m_currentScene : m_currentScene->FindChild(m_sceneNodeId, true);
    if (!selectedNode)
    {
        m_sceneNodeId = NXS_INVALID_ID;
        return;
    }

    selectedNode->AcceptReflector(CAST<IReflector&>(*this));
}

void PropertyWindow::SetMarker(const std::string& name)
{
    if (const auto hash = Hasher().Hash32(name); hash != m_currentCategory)
    {
        ImGui::SeparatorText(name.c_str());
        m_currentCategory = hash;
    }
}

void PropertyWindow::SetReadOnlyFlag(bool value)
{
    if (value) ImGui::BeginDisabled();
    else ImGui::EndDisabled();
}

bool PropertyWindow::VisitBool_Implement(const std::string& name, bool& value)
{
    return ImGui::Checkbox(name.c_str(), &value);
}

bool PropertyWindow::VisitInt_Implement(const std::string& name, int32_t& value)
{
    return ImGui::InputInt(name.c_str(), &value);
}

bool PropertyWindow::VisitUInt_Implement(const std::string& name, uint32_t& value)
{
    return ImGui::InputScalar(name.c_str(), ImGuiDataType_U32, &value);
}

bool PropertyWindow::VisitInt64_Implement(const std::string& name, int64_t& value)
{
    return ImGui::InputScalar(name.c_str(), ImGuiDataType_S64, &value);
}

bool PropertyWindow::VisitUInt64_Implement(const std::string& name, uint64_t& value)
{
    return ImGui::InputScalar(name.c_str(), ImGuiDataType_U64, &value);
}

bool PropertyWindow::VisitFloat_Implement(const std::string& name, float& value)
{
    return ImGui::InputFloat(name.c_str(), &value);
}

bool PropertyWindow::VisitDouble_Implement(const std::string & name, double & value)
{
    return ImGui::InputDouble(name.c_str(), &value);
}

bool PropertyWindow::VisitString_Implement(const std::string& name, std::string& value)
{
    char buf[2048] = "";
    const auto size = sizeof(buf);
    NXS_STRNCPY(buf, size, value.c_str(), value.length());

    if (ImGui::InputText(name.c_str(), buf, size)) {
        value = buf;
        return true;
    }
    return false;
}

bool PropertyWindow::VisitVec2_Implement(const std::string& name, glm::vec2& value)
{
    return ImGui::InputFloat2(name.c_str(), R_CAST<float*>(&value));
}

bool PropertyWindow::VisitVec3_Implement(const std::string& name, glm::vec3& value)
{
    return ImGui::InputFloat3(name.c_str(), R_CAST<float*>(&value));
}

bool PropertyWindow::VisitVec4_Implement(const std::string& name, glm::vec4& value)
{
    return ImGui::InputFloat4(name.c_str(), R_CAST<float*>(&value));
}

bool PropertyWindow::VisitColor3_Implement(const std::string& name, Color3F& value)
{
    return ImGui::ColorPicker3(name.c_str(), R_CAST<float*>(&value));
}

bool PropertyWindow::VisitColor4_Implement(const std::string& name, Color4F& value)
{
    return ImGui::ColorPicker3(name.c_str(), R_CAST<float*>(&value));
}

bool PropertyWindow::VisitObject_Implement(const std::string &name, IReflection&value)
{
    SetMarker(name.c_str());
    bool valueChanged = false;
    auto connection = onValueChangedEvent.connect([&valueChanged](const std::string& name) {
        valueChanged = true;
    });
    value.AcceptReflector(*this);
    connection.disconnect();
    return valueChanged;
}

MAYBE_UNUSED bool PropertyWindow::VisitEnum_Implement(const std::string &name, int32_t &value, const Array<std::string> &enumStrings)
{
    std::vector<const char*> items;
    items.reserve(enumStrings.size());
    for (const auto& item : enumStrings)
    {
        items.push_back(item.c_str());
    }
    return ImGui::Combo(name.c_str(), &value, items.data(), static_cast<int>(items.size()));
}
