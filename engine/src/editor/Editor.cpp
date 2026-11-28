//
// Created by nutta on 7/9/2025.
//

#include "editor/Editor.h"
#include "nexus/graphics/RenderSystem.h"
#include "Application.h"

#include "tracy/Tracy.hpp"

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
    ZoneScopedN("Editor_Update");
    m_menu->Update();
    UpdateWidgets();
}

void Editor::Draw(RenderSystem& renderSystem)
{
    ZoneScopedN("Editor_Draw");
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
    ZoneScopedN("Editor_UpdateWidgets");
    for (const auto& widget : m_widgets)
    {
        widget->Update();
    }
}

void Editor::DrawWidgets(RenderSystem& renderSystem) const
{
    ZoneScopedN("Editor_DrawWidgets");
    for (const auto& widget : m_widgets)
    {
        widget->Draw(renderSystem);
    }
}
