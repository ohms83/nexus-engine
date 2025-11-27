#include "editor/widget/SceneGraphWidget.h"

#include "imgui.h"

USING_NAMESPACE_NXS;
SceneGraphWidget::SceneGraphWidget(SceneManager& sceneManager)
    : EditorWidget("Scene Graph")
{
    SetCurrentScene(sceneManager.GetCurrentScene());
    sceneManager.sceneChangedCallback.connect([this] (Ref<Scene> prev, Ref<Scene> current) {
        SetCurrentScene(current);
    });
}

SceneGraphWidget::~SceneGraphWidget()
{
    m_scene.reset();
}

void SceneGraphWidget::SetCurrentScene(Ref<Scene> scene)
{
    m_scene = scene;
    m_selectedNode = m_scene ? m_scene->GetId() : InvalidID;
}

Identifier SceneGraphWidget::GetSelectedNode() const
{
    return m_selectedNode;
}

void SceneGraphWidget::Draw_Internal(RenderSystem& renderSystem)
{
    DrawSceneNode(m_scene);
    ShowContextMenu();
}

void SceneGraphWidget::DrawSceneNode(Ref<SceneNode> node)
{
    ImGui::PushID(&node);

    const auto nodeId = node->GetId();
    const void* treeNodeID = (void*)((intptr_t)nodeId);
    ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_SpanAvailWidth;
    nodeFlags |= node->HasChild() ? ImGuiTreeNodeFlags_OpenOnArrow : ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet;

    if (nodeId == m_selectedNode)
    {
        ImGui::PushStyleColor(ImGuiCol_Header, IM_COL32(127, 127, 80, 255));
        nodeFlags |= ImGuiTreeNodeFlags_Selected;
    }

    const bool isOpen = ImGui::TreeNodeEx(treeNodeID, nodeFlags, "%s", node->GetName().c_str());

    // Pop the custom color immediately after drawing the node.
    if (nodeId == m_selectedNode)
    {
        ImGui::PopStyleColor();
    }

    HandleInput(node);

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
    m_scene->RemoveChild(node);
}

void SceneGraphWidget::HandleInput(Ref<SceneNode> node)
{
    if (ImGui::IsItemClicked(ImGuiMouseButton_Left) || ImGui::IsItemClicked(ImGuiMouseButton_Right))
    {
        m_selectedNode = node->GetId();
    }
}

void SceneGraphWidget::ShowContextMenu()
{
    if (m_selectedNode == InvalidID) return;
    static const char* contexMenuID = "Scene Graph Context Menu";

    auto node = m_scene->FindNode(m_selectedNode);
    if (!node) return;

    bool closePopup = false;

    if (ImGui::BeginPopupContextWindow(contexMenuID, ImGuiPopupFlags_MouseButtonRight))
    {
        if (ImGui::MenuItem("Delete"))
        {
            DeleteNode(node);
            closePopup = true;
        }
        if (ImGui::MenuItem("Cancel"))
        {
            closePopup = true;
        }
        ImGui::EndPopup();
    }
    
    if (closePopup)
    {
        m_selectedNode = InvalidID;
    }
}