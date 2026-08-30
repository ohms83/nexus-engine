#include "editor/Menu.h"
#include "editor/Editor.h"
#include "editor/FileDialog.h"
#include "editor/menu/ToggleMenuItem.h"
#include "editor/menu/TriggerMenuItem.h"
#include "editor/menu/WidgetMenuItem.h"
#include "editor/menu/ConsoleMenuItem.h"
#include "editor/menu/SceneFileMenuItem.h"
#include "editor/utils/ModelImporter.h"
#include "editor/widget/EditorWidget.h"
#include "editor/widget/ProfilerWidget.h"
#include "editor/widget/PropertyWindow.h"
#include "nexus/debug/LogDispatcher.h"
#include "nexus/serialize/JsonSerializer.h"

#include "Application.h"

#include "DebugMenuItem.inl"

#include "imgui.h"

#include <fstream>

USING_NAMESPACE_NXS;

#define FILE_MENU_OPEN_SAVE 0
#define FILE_MENU_QUIT_APP 1

#define EDIT_UNDO_REDO 0
#define EDIT_COPY_PASTE 1

#define DEVELOPER_TOOLS 0

DEFINE_LOG(Menu);

Menu::Menu(IWidgetOwner& widgetOwner)
{
    const auto& application = CAST<Editor*>(&widgetOwner)->GetParentApp();
    const auto& engine = Engine::Instance();

    auto newSceneMenu = std::make_shared<TriggerMenuItem> (
        FILE_MENU_OPEN_SAVE,
        "New Scene",
        "",
        "Ctrl+N",
        [&widgetOwner]() {
            auto& application = CAST<Editor*>(&widgetOwner)->GetParentApp();
            auto& sceneManager = application.GetSceneManager();
            sceneManager.EmplaceAndChange<Scene>("New Scene");
        }
    );
    auto importSceneMenu = std::make_shared<SceneImportMenuItem>(
        FILE_MENU_OPEN_SAVE,
        application.GetWindowContext()
    );
    auto openSceneMenu = std::make_shared<SceneFileMenuItem>(
        application.GetSceneManager(),
        std::make_shared<JsonSerializer>(),
        FILE_MENU_OPEN_SAVE,
        "Open Scene",
        "Open an existing scene.",
        "Ctrl+O",
        FileMenuContext {
            .windowContext = application.GetWindowContext(),
            .filters = { {"Nexus scene file (*.nxs)", "*.nxs"} },
            .defaultExtension = "nxs",
            .dialogMode = FileDialogContext::Mode::Open,
        }
    );
    auto saveSceneMenu = std::make_shared<SceneFileMenuItem>(
        application.GetSceneManager(),
        std::make_shared<JsonSerializer>(),
        FILE_MENU_OPEN_SAVE,
        "Save Scene",
        "Save the currently active scene.",
        "Ctrl+S",
        FileMenuContext {
            .windowContext = application.GetWindowContext(),
            .filters = { {"Nexus scene file (*.nxs)", "*.nxs"} },
            .defaultExtension = "nxs",
            .dialogMode = FileDialogContext::Mode::Save,
        }
    );
    auto quitMenu = std::make_shared<TriggerMenuItem> (
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
    );
    MenuItemList fileMenu = {
        "File",
        {
            // New Scene
            newSceneMenu,
            // Open Scene
            openSceneMenu,
            // Save Scene
            saveSceneMenu,
            // Import Scene
            importSceneMenu,
            // Quit Application
            quitMenu,
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
        "Window",
        {
            std::make_shared<ConsoleMenuItem> (
                DEVELOPER_TOOLS,
                "Console",
                "",
                "",
                std::make_shared<Console>(),
                widgetOwner,
                LogDispatcher::Instance()
            ),
            std::make_shared<WidgetMenuItem> (
                DEVELOPER_TOOLS,
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
    m_menuItems.emplace_back(s_debugMenuItems);
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

Ref<MenuItem> Menu::GetMenuItem(const std::string& menu, const std::string& name)
{
    const auto itr = std::ranges::find_if(m_menuItems, [&menu] (const MenuItemList& itemList)
    {
        return menu == itemList.menu;
    });

    if (itr == m_menuItems.end()) return nullptr;

    const auto& items = itr->items;
    const auto menuItr = std::ranges::find_if(items, [&name] (const Ref<MenuItem>& item)
    {
        return item->GetName() == name;
    });

    return (menuItr == items.end()) ? nullptr : *menuItr;
}

void Menu::DrawMenu()
{
    if (ImGui::BeginMainMenuBar())
    {
        for (const auto& [menu, items] : m_menuItems)
        {
            if (ImGui::BeginMenu(menu.c_str()))
            {
                int32_t group = -1;
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
                    group = CAST<int32_t>(item->GetGroup());
                }
                ImGui::EndMenu();
            }
        }

        ImGui::EndMainMenuBar(); // End the main menu bar
    }
}