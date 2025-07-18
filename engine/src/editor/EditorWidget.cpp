//
// Created by nutta on 7/18/2025.
//

#include <nexus/editor/EditorWidget.h>
#include "imgui.h"

USING_NAMESPACE_NXS;

void EditorWidget::Draw(const RenderSystem& renderSystem)
{
    BeginDraw(renderSystem);
    if (visible) {
        Draw_Internal(renderSystem);
    }
    EndDraw();
}

void EditorWidget::BeginDraw(const RenderSystem& renderSystem)
{
    if (!ImGui::Begin(m_name.data(), &visible)) {
        ImGui::End();
    }
}

void EditorWidget::EndDraw()
{
    ImGui::End();
}
