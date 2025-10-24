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
        auto comp = selectedNode->TryGetComponent<OrientationComponent>();
        if (comp)
        {
            const glm::vec3 orgRadians = glm::eulerAngles(comp->value);
            glm::vec3 eulerAngles = glm::degrees(orgRadians);
            if (ImGui::InputFloat3("Euler Angles", R_CAST<float*>(&eulerAngles)))
            {
                // const auto radians = glm::radians(eulerAngles);
                // const float tolerance = 0.00001f;
                // if (!glm::all(glm::epsilonEqual(radians, orgRadians, tolerance)))
                // {
                //     // Create a 4x4 matrix representing the combined rotation (order: X, then Y, then Z)
                //     // Note: GLM's eulerAngle functions often use 4x4 matrices
                //     glm::mat4 rotationMatrix = glm::eulerAngleXYZ(eulerAngles.x, eulerAngles.y, eulerAngles.z);
                //     // Convert the 4x4 matrix (which is a 3x3 rotation plus translation) to a quaternion
                //     comp->value = glm::quat(rotationMatrix);
                //     LOG_DEBUG(LogTemp, std::format("New orient: radians={} euler={}",
                //          math::ToString(radians),  math::ToString(eulerAngles)));
                // }
            }
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