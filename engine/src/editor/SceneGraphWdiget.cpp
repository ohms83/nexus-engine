#include "editor/SceneGraphWidget.h"

#include "imgui.h"

USING_NAMESPACE_NXS;

SceneGraphWidget::~SceneGraphWidget()
{
    m_scene.reset();
    m_highlightNode.reset();
    m_selectedNode.reset();
}

void SceneGraphWidget::Draw_Internal(RenderSystem& renderSystem)
{
    m_highlightNode.reset();

    SceneNode::ChildList rootNodes;
    rootNodes.clear();
    m_scene->GetAllRootNodes(rootNodes);
    for (const auto node : rootNodes)
    {
        DrawSceneNode(node);
    }

    // Check for user interactions (e.g., right-click context menu)
    static const std::string contexMenuID = "node_context_menu";
    if (m_highlightNode && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
    {
        ImGui::OpenPopup(contexMenuID.c_str());
        m_selectedNode = m_highlightNode;
    }

    if (ImGui::BeginPopup(contexMenuID.c_str()))
    {
        if (ImGui::MenuItem("Delete"))
        {
            DeleteNode(m_selectedNode);
            m_selectedNode.reset();
        }
        ImGui::EndPopup();
    }
    else if (m_selectedNode)
    {
        m_selectedNode.reset();
    }
}

void SceneGraphWidget::DrawSceneNode(Ref<SceneNode> node)
{
    ImGui::PushID(&node);
    bool isOpen = ImGui::TreeNode(node->GetName().c_str());

    if (ImGui::IsItemHovered())
    {
        m_highlightNode = node;
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

void SceneGraphWidget::DeleteNode(Ref<SceneNode> node)
{
    if (!node) return;
    m_scene->RemoveNode(node);
}