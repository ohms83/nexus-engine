//
// Created by nutta on 7/9/2025.
//

#include "editor/Editor.h"
#include "nexus/graphics/RenderSystem.h"
#include "Application.h"

#include "Remotery.h"

USING_NAMESPACE_NXS;

DEFINE_LOG(Editor);

Editor::Editor(Application& parentApp)
    : m_parentApp(parentApp)
{
    m_menu = std::make_unique<Menu>(*this);
}

Editor::~Editor()
{
    Clear();
}

WindowContext Editor::GetWindowContext() const
{
    return m_parentApp.GetWindowContext();
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
