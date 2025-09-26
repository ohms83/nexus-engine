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
    InitMenu();
}

Editor::~Editor()
{
    Clear();
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void Editor::Update() const
{
    for (const auto& widget : m_widgets)
    {
        widget->Update();
    }
}

void Editor::Draw(RenderSystem& renderSystem)
{
    rmt_ScopedCPUSample(Editor_Draw, 0);
    DrawMainMenu();

    for (const auto& widget : m_widgets)
    {
        widget->Draw(renderSystem);
    }
}

void Editor::AddMenuItem(const std::string& menu, const MenuItem& menuItem)
{
    const auto itr = std::ranges::find_if(m_menuItems, [&menu] (const MenuItemList& itemList)
    {
        return menu == itemList.menu;
    });
    if (itr == m_menuItems.end())
    {
        MenuItemList itemList = {
            menu,
            {menuItem}
        };
        m_menuItems.emplace_back(itemList);
    }
    else
    {
        itr->items.push_back(menuItem);
    }

    if (menuItem.widget)
    {
        m_widgets.push_back(menuItem.widget);
    }
}

void Editor::Clear()
{
    m_menuItems.clear();
    m_widgets.clear();
}

void Editor::InitMenu()
{
    std::vector<MenuItem> fileMenuItems = {
        {
            FILE_MENU_OPEN_SAVE,
            "New",
            "",
            "Ctrl+N",
            nullptr,
            [](const MenuItem&)
            {
                LOG_INFO(LogEditor, "TODO: Open a file creating dialogue");
            }
        },
        {
            FILE_MENU_OPEN_SAVE,
            "Open...",
            "",
            "Ctrl+O",
            nullptr,
            [](const MenuItem&)
            {
                LOG_INFO(LogEditor, "TODO: Open a file opening dialogue");
            }
        },
        {
            FILE_MENU_OPEN_SAVE,
            "Save",
            "",
            "Ctrl+S",
            nullptr,
            [](const MenuItem&)
            {
                LOG_INFO(LogEditor, "TODO: Save a file");
            }
        },
        {
            FILE_MENU_QUIT_APP,
            "Quit",
            "",
            "Alt+F4",
            nullptr,
            [](const MenuItem&)
            {
                SDL_QuitEvent quitEvent {
                    SDL_EVENT_QUIT,
                    0,
                    SDL_GetTicksNS()
                };
                SDL_PushEvent(R_CAST<SDL_Event*>(&quitEvent));
            }
        }
    };

    std::vector<MenuItem> editMenuItems = {
        {
            EDIT_UNDO_REDO,
            "Undo",
            "",
            "Ctrl+Z",
            nullptr,
            [](const MenuItem&)
            {
                LOG_INFO(LogEditor, "TODO: Undo");
            }
        },
        {
            EDIT_UNDO_REDO,
            "Redo",
            "",
            "Ctrl+Y",
            nullptr,
            [](const MenuItem&)
            {
                LOG_INFO(LogEditor, "TODO: Redo");
            }
        },
        {
            EDIT_COPY_PASTE,
            "Cut",
            "",
            "Ctrl+X",
            nullptr,
            [](const MenuItem&)
            {
                LOG_INFO(LogEditor, "TODO: Cut");
            }
        },
        {
            EDIT_COPY_PASTE,
            "Copy",
            "",
            "Ctrl+C",
            nullptr,
            [](const MenuItem&)
            {
                LOG_INFO(LogEditor, "TODO: Copy");
            }
        },
        {
            EDIT_COPY_PASTE,
            "Paste",
            "",
            "Ctrl+V",
            nullptr,
            [](const MenuItem&)
            {
                LOG_INFO(LogEditor, "TODO: Paste");
            }
        }
    };

    // Console needs another initialization step, so it can't be put in the list.
    MenuItem console = {
        DEVELOPER_TOOLS,
        "Console",
        "Open a debug console",
        "",
        std::make_shared<Console>(),
        {}
    };
    LogDispatcher::Instance().AddLogger(PTR_CAST<ILogger>(console.widget));

    std::vector<MenuItem> toolsMenuItems = {
        {
            DEVELOPER_TOOLS,
            "Profiler",
            "Open a profiler",
            "",
            std::make_shared<ProfilerWidget>()
        }
    };
    toolsMenuItems.insert(toolsMenuItems.begin(), console);

    for (const auto& menuItem : fileMenuItems)
    {
        AddMenuItem("File", menuItem);
    }
    for (const auto& menuItem : editMenuItems)
    {
        AddMenuItem("Edit", menuItem);
    }
    for (const auto& menuItem : toolsMenuItems)
    {
        AddMenuItem("Tools", menuItem);
    }
    for (const auto& menuItem : s_debugMenuItems)
    {
        AddMenuItem("Debug", menuItem);
    }
}

void Editor::DrawMainMenu()
{
    if (ImGui::BeginMainMenuBar()) {
        for (const auto& [menu, items] : m_menuItems)
        {
            if (ImGui::BeginMenu(menu.c_str()))
            {
                int32 group = -1;
                for (const auto& item : items)
                {
                    if (group != -1 && group != item.group)
                    {
                        ImGui::Separator(); // Adds a horizontal line separator
                    }

                    // ReSharper disable once CppTooWideScopeInitStatement
                    bool selected = item.widget ? item.widget->GetVisibility() != EditorWidget::Visibility::Hidden : false;
                    if (ImGui::MenuItem(item.name.c_str(), item.shortcut.c_str(), &selected))
                    {
                        if (item.handler) item.handler(item);
                    }
                    if (item.widget)
                    {
                        if (selected)
                        {
                            item.widget->Show();
                        }
                        else
                        {
                            item.widget->Hide();
                        }
                    }
                    group = CAST<int32>(item.group);
                }
                ImGui::EndMenu();
            }
        }

        ImGui::EndMainMenuBar(); // End the main menu bar
    }
}
