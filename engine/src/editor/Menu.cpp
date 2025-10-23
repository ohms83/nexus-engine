#include "editor/Menu.h"
#include "editor/EditorWidget.h"
#include "editor/ProfilerWidget.h"
#include "editor/menu/ToggleMenuItem.h"
#include "editor/menu/TriggerMenuItem.h"
#include "editor/menu/WidgetMenuItem.h"
#include "editor/menu/ConsoleMenuItem.h"

#include "imgui.h"

USING_NAMESPACE_NXS;

#define FILE_MENU_OPEN_SAVE 0
#define FILE_MENU_QUIT_APP 1

#define EDIT_UNDO_REDO 0
#define EDIT_COPY_PASTE 1

#define DEVELOPER_TOOLS 0

DEFINE_LOG(Menu);

Menu::Menu(IWidgetOwner& widgetOwner)
{
    MenuItemList fileMenu = {
        "File",
        {
            std::make_shared<TriggerMenuItem> (
                FILE_MENU_OPEN_SAVE,
                "New",
                "",
                "Ctrl+N",
                []() {

                }
            ),
            std::make_shared<TriggerMenuItem> (
                FILE_MENU_OPEN_SAVE,
                "Open...",
                "",
                "Ctrl+O",
                []() {

                }
            ),
            std::make_shared<TriggerMenuItem> (
                FILE_MENU_OPEN_SAVE,
                "Save",
                "",
                "Ctrl+S",
                []() {

                }
            ),
            std::make_shared<TriggerMenuItem> (
                FILE_MENU_QUIT_APP,
                "Quit",
                "",
                "Alt+F4",
                []() {
                    SDL_QuitEvent quitEvent {
                        SDL_EVENT_QUIT,
                        0,
                        SDL_GetTicksNS()
                    };
                    SDL_PushEvent(R_CAST<SDL_Event*>(&quitEvent));
                }
            ),
        }
    };

    MenuItemList editMenu = {
        "Edit",
        {
            std::make_shared<TriggerMenuItem> (
                EDIT_UNDO_REDO,
                "Undo",
                "",
                "Ctrl+Z",
                []() {
                    LOG_INFO(LogMenu, "TODO: Undo");
                }
            ),
            std::make_shared<TriggerMenuItem> (
                EDIT_UNDO_REDO,
                "Redo",
                "",
                "Ctrl+Y",
                []() {
                    LOG_INFO(LogMenu, "TODO: Redo");
                }
            ),
            std::make_shared<TriggerMenuItem> (
                EDIT_COPY_PASTE,
                "Cut",
                "",
                "Ctrl+X",
                []() {
                    LOG_INFO(LogMenu, "TODO: Cut");
                }
            ),
            std::make_shared<TriggerMenuItem> (
                EDIT_COPY_PASTE,
                "Copy",
                "",
                "Ctrl+C",
                []() {
                    LOG_INFO(LogMenu, "TODO: Copy");
                }
            ),
            std::make_shared<TriggerMenuItem> (
                EDIT_COPY_PASTE,
                "Paste",
                "",
                "Ctrl+V",
                []() {
                    LOG_INFO(LogMenu, "TODO: Paste");
                }
            ),
        }
    };

    MenuItemList toolsMenuItems = {
        "Tools",
        {
            std::make_shared<ConsoleMenuItem> (
                EDIT_UNDO_REDO,
                "Console",
                "",
                "",
                std::make_shared<Console>(),
                widgetOwner,
                LogDispatcher::Instance()
            ),
            std::make_shared<WidgetMenuItem> (
                EDIT_UNDO_REDO,
                "Profiler",
                "",
                "",
                std::make_shared<ProfilerWidget>(),
                widgetOwner
            ),
        }
    };

    m_menuItems.emplace_back(fileMenu);
    m_menuItems.emplace_back(editMenu);
    m_menuItems.emplace_back(toolsMenuItems);
}

void Menu::Update() const
{
    for (auto widget : m_widgets)
    {
        widget->Update();
    }
}

void Menu::Draw(RenderSystem& renderSystem)
{
    DrawMenu();
}

void Menu::AddMenuItem(const std::string& menu, Ref<MenuItem> menuItem)
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

    const auto widgetMenuItem = PTR_CAST<WidgetMenuItem>(menuItem);
    if (widgetMenuItem)
    {
        m_widgets.push_back(widgetMenuItem->GetWidget());
    }
}

void Menu::DrawMenu()
{
    if (ImGui::BeginMainMenuBar())
    {
        for (const auto& [menu, items] : m_menuItems)
        {
            if (ImGui::BeginMenu(menu.c_str()))
            {
                int32 group = -1;
                for (const auto item : items)
                {
                    if (group != -1 && group != item->GetGroup())
                    {
                        ImGui::Separator(); // Adds a horizontal line separator
                    }

                    bool selected = item->IsSelected();
                    if (ImGui::MenuItem(item->GetName().c_str(), item->GetShortcut().c_str(), selected))
                    {
                        item->OnSelected(!selected);
                    }
                    group = CAST<int32>(item->GetGroup());
                }
                ImGui::EndMenu();
            }
        }

        ImGui::EndMainMenuBar(); // End the main menu bar
    }
}