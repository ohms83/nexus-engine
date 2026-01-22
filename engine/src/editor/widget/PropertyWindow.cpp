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

bool PropertyWindow::VisitBool(const std::string& name, bool& value)
{
    return ImGui::Checkbox(name.c_str(), &value);
}

bool PropertyWindow::VisitInt(const std::string& name, int32_t& value)
{
    return ImGui::InputInt(name.c_str(), &value);
}

bool PropertyWindow::VisitUInt(const std::string& name, uint32_t& value)
{
    return ImGui::InputInt(name.c_str(), R_CAST<int*>(&value));
}

bool PropertyWindow::VisitInt64(const std::string& name, int64_t& value)
{
    NXS_ASSERT_MSG(value > INT32_MAX, std::format("Integer value overflow - {}: {}", name, value));
    return ImGui::InputInt(name.c_str(), R_CAST<int*>(&value));
}

bool PropertyWindow::VisitUInt64(const std::string& name, uint64_t& value)
{
    NXS_ASSERT_MSG(value > INT32_MAX, std::format("Integer value overflow - {}: {}", name, value));
    return ImGui::InputInt(name.c_str(), R_CAST<int*>(&value));
}

bool PropertyWindow::VisitFloat(const std::string& name, float& value)
{
    return ImGui::InputFloat(name.c_str(), &value);
}

bool PropertyWindow::VisitDouble(const std::string & name, double & value)
{
    return ImGui::InputDouble(name.c_str(), &value);
}

bool PropertyWindow::VisitString(const std::string& name, std::string& value)
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

bool PropertyWindow::VisitVec2(const std::string& name, glm::vec2& value)
{
    return ImGui::InputFloat2(name.c_str(), R_CAST<float*>(&value));
}

bool PropertyWindow::VisitVec3(const std::string& name, glm::vec3& value)
{
    return ImGui::InputFloat3(name.c_str(), R_CAST<float*>(&value));
}

bool PropertyWindow::VisitVec4(const std::string& name, glm::vec4& value)
{
    return ImGui::InputFloat4(name.c_str(), R_CAST<float*>(&value));
}

bool PropertyWindow::VisitColor3(const std::string& name, Color3F& value)
{
    return ImGui::ColorPicker3(name.c_str(), R_CAST<float*>(&value));
}

bool PropertyWindow::VisitColor4(const std::string& name, Color4F& value)
{
    return ImGui::ColorPicker3(name.c_str(), R_CAST<float*>(&value));
}

bool PropertyWindow::VisitObject(const std::string &name, IReflector &value)
{
    // Property window doesn't support nested object so it will be silently ignored.
    return false;
}
