#include "editor/SceneGraphWidget.h"

#include "imgui.h"

USING_NAMESPACE_NXS;

void SceneGraphWidget::Draw_Internal(RenderSystem& renderSystem)
{
    m_selectedNode.reset();

    SceneNode::ChildList rootNodes;
    rootNodes.clear();
    m_scene->GetAllRootNodes(rootNodes);
    for (const auto node : rootNodes)
    {
        DrawSceneNode(node);
    }

    // Check for user interactions (e.g., right-click context menu)
    static const std::string contexMenuID = "node_context_menu";
    if (m_selectedNode && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
    {
        ImGui::OpenPopup(contexMenuID.c_str());
    }

    if (ImGui::BeginPopup(contexMenuID.c_str()))
    {
        if (ImGui::MenuItem("Delete")) {
            // Logic to delete the node
        }
        ImGui::EndPopup();
    }
}

void SceneGraphWidget::DrawSceneNode(Ref<const SceneNode> node)
{
    ImGui::PushID(&node);
    bool isOpen = ImGui::TreeNode(node->GetName().c_str());

    if (ImGui::IsItemHovered())
    {
        m_selectedNode = node;
    }

    if (isOpen)
    {
        SceneNode::ChildList children;
        node->GetAllChildren(children);
        for (const auto& child : children)
        {
            DrawSceneNode(child);
        }
        ImGui::TreePop();
    }
    
    ImGui::PopID();
}