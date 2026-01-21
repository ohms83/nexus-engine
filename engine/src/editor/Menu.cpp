#include "editor/Menu.h"
#include "editor/Editor.h"
#include "editor/FileDialog.h"
#include "editor/menu/ToggleMenuItem.h"
#include "editor/menu/TriggerMenuItem.h"
#include "editor/menu/WidgetMenuItem.h"
#include "editor/menu/ConsoleMenuItem.h"
#include "editor/widget/EditorWidget.h"
#include "editor/widget/ProfilerWidget.h"
#include "editor/widget/PropertyWindow.h"
#include "core/LogDispatcher.h"
#include "io/JsonSerializer.h"

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
    MenuItemList fileMenu = {
        "File",
        {
            std::make_shared<TriggerMenuItem> (
                FILE_MENU_OPEN_SAVE,
                "New Scene",
                "",
                "Ctrl+N",
                []() {
                }
            ),
            std::make_shared<TriggerMenuItem> (
                FILE_MENU_OPEN_SAVE,
                "Open Scene",
                "Open an existing scene.",
                "Ctrl+O",
                [&widgetOwner]() {
                    FileDialogContext context {
                        .windowContext = widgetOwner.GetWindowContext(),
                        .filters = { {"Nexus scene file (*.nxs)", "*.nxs"} },
                        .title = "Open Scene",
                        .defaultExtension = "nxs",
                        .mode = FileDialogContext::Mode::Open,
                    };
                    // TODO:
                    (void)ShowFileDialog(context);
                }
            ),
            std::make_shared<TriggerMenuItem> (
                FILE_MENU_OPEN_SAVE,
                "Save Scene",
                "",
                "Ctrl+S",
                [&widgetOwner]() {
                    FileDialogContext context {
                        .windowContext = widgetOwner.GetWindowContext(),
                        .filters = { {"Nexus scene file (*.nxs)", "*.nxs"} },
                        .title = "Save Scene",
                        .defaultExtension = "nxs",
                        .mode = FileDialogContext::Mode::Save,
                    };
                    // TODO: Move the serialization code to a separate file.
                    const auto filepath = ShowFileDialog(context);
                    LOG_INFO(LogTemp, std::format("Filepath={}", filepath));
                    if (filepath.empty()) return;

                    std::ofstream file(filepath);
                    if (!file.good()) return;

                    auto serializer = JsonSerializer();
                    auto& application = CAST<Editor*>(&widgetOwner)->GetParentApp();
                    auto& sceneManager = application.GetSceneManager();
                    auto scene = sceneManager.GetCurrentScene();
                    serializer.Pack(scene->Serialize(), file);
                }
            ),
            std::make_shared<TriggerMenuItem> (
                FILE_MENU_OPEN_SAVE,
                "Import",
                "Import a 3D model to the currently active scene.",
                "Ctrl+I",
                [&widgetOwner]() {
                    FileDialogContext context {
                        .windowContext = widgetOwner.GetWindowContext(),
                        .title = "Import Model",
                        .mode = FileDialogContext::Mode::Open,
                    };
                    // TODO:
                    (void)ShowFileDialog(context);
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