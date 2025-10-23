//
// Created by nutta on 7/9/2025.
//

#include <nexus/editor/Editor.h>
#include <nexus/graphics/RenderSystem.h>

// ImGui headers
#include <imgui.h>

#include "editor/Console.h"
#include "editor/ProfilerWidget.h"
#include "editor/DebugMenuItem.h"

#include "Remotery.h"

USING_NAMESPACE_NXS;

#define FILE_MENU_OPEN_SAVE 0
#define FILE_MENU_QUIT_APP 1

#define EDIT_UNDO_REDO 0
#define EDIT_COPY_PASTE 1

#define DEVELOPER_TOOLS 0

DEFINE_LOG(Editor);

Editor::Editor()
{
    m_menu = std::make_unique<Menu>(*this);
}

Editor::~Editor()
{
    Clear();
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void Editor::Update()
{
    rmt_ScopedCPUSample(Editor_Update, 0);
    m_menu->Update();
    UpdateWidgets();
}

void Editor::Draw(RenderSystem& renderSystem)
{
    rmt_ScopedCPUSample(Editor_Draw, 0);
    m_menu->Draw(renderSystem);
    DrawWidgets(renderSystem);
}

void Editor::Clear()
{
    m_menu.reset();
    m_widgets.clear();
}

void Editor::UpdateWidgets()
{
    rmt_ScopedCPUSample(Editor_UpdateWidgets, 0);
    for (const auto& widget : m_widgets)
    {
        widget->Update();
    }
}

void Editor::DrawWidgets(RenderSystem& renderSystem) const
{
    rmt_ScopedCPUSample(Editor_DrawWidgets, 0);
    for (const auto& widget : m_widgets)
    {
        widget->Draw(renderSystem);
    }
}
