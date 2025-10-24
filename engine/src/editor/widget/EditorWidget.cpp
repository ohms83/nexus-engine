//
// Created by nutta on 7/18/2025.
//

#include "editor/widget/EditorWidget.h"
#include "imgui.h"

USING_NAMESPACE_NXS;

void EditorWidget::Draw(RenderSystem& renderSystem)
{
    if (m_visibility == Visibility::Hidden) {
        return;
    }

    BeginDraw();
    Draw_Internal(renderSystem);
    EndDraw();
}

void EditorWidget::Show()
{
    m_visible = true;
    m_visibility = m_visibility == Visibility::Hidden ? Visibility::Visible : m_visibility;
}

void EditorWidget::Hide()
{
    m_visible = false;
    m_visibility = Visibility::Hidden;
}

void EditorWidget::BeginDraw()
{
    const bool isCollapsed = !ImGui::Begin(m_name.data(), &m_visible);
    if (m_visible) {
        m_visibility = isCollapsed ? Visibility::Collapsed : Visibility::Visible;
    }
    else {
        m_visibility = Visibility::Hidden;
    }
}

void EditorWidget::EndDraw()
{
    ImGui::End();
}
