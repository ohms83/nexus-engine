//
// Created by nutta on 7/9/2025.
//

#include <nexus/editor/Editor.h>
#include <nexus/graphics/RenderSystem.h>

// ImGui headers
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_opengl3.h>

#include <implot.h>
#include <iostream>

#include "core/Logger.h"
#include "nexus/editor/Console.h"
#include "nexus/editor/Profiler.h"

USING_NAMESPACE_NXS;

#define FILE_MENU_OPEN_SAVE 0
#define FILE_MENU_QUIT_APP 1

#define EDIT_UNDO_REDO 0
#define EDIT_COPY_PASTE 1

#define DEVELOPER_TOOLS 0

static const std::string EditorLog = "Editor";

Editor::Editor(const WindowContext window, const RenderContext renderContext, const EditorConfig& config)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;    // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;       // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;     // Enable Multi-Viewport / Platform Windows
    io.FontGlobalScale = 1.5f;
    io.ConfigDpiScaleFonts = true;                          // [Experimental] Automatically overwrite style.FontScaleDpi in Begin() when Monitor DPI changes. This will scale fonts but _NOT_ scale sizes/padding for now.
    io.ConfigDpiScaleViewports = true;                      // [Experimental] Scale Dear ImGui and Platform Windows when Monitor DPI changes.

    // Setup ImGui style
    ImGui::StyleColorsDark();
    // When viewports are enabled, tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    // --- Make windows transparent ---
    style.Colors[ImGuiCol_WindowBg].w = 0.5f;

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 0.5f;
    }

    // You might also want to adjust the title bar background if it's too opaque
    style.Colors[ImGuiCol_TitleBg].w = 0.8f;
    style.Colors[ImGuiCol_TitleBgActive].w = 0.85f; // Active title bar
    style.Colors[ImGuiCol_TitleBgCollapsed].w =.7f; // Collapsed title bar

    m_config = config;
    if (m_config.renderingBackend == GraphicsAPI::OpenGL)
    {
        // Setup Platform/Renderer backends
        ImGui_ImplSDL3_InitForOpenGL(window, renderContext.gl_context);
        ImGui_ImplOpenGL3_Init(nullptr); // Match your core profile version
    }
    else
    {
        // Not yet implemented
        assert(false);
    }

    // Initializes ImPlot
    ImPlot::CreateContext();

    InitMenu();
}

Editor::~Editor()
{
    ImPlot::DestroyContext();

    if (m_config.renderingBackend == GraphicsAPI::OpenGL)
    {
        ImGui_ImplOpenGL3_Shutdown();
    }

    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void Editor::Update(const SDL_Event& event)
{
    ImGui_ImplSDL3_ProcessEvent(&event);

    for (const auto widget : m_widgets)
    {
        widget->Update();
    }
}

void Editor::BeginDraw() const
{
    if (m_config.renderingBackend == GraphicsAPI::OpenGL)
    {
        ImGui_ImplOpenGL3_NewFrame();
    }
    else
    {
        // Not yet implemented
        assert(false);
    }
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
}

void Editor::Draw(const RenderSystem& renderSystem)
{
    DrawMainMenu(renderSystem);

    for (const auto widget : m_widgets)
    {
        if (widget && widget->visible) {
            widget->Draw(renderSystem);
        }
    }
}

void Editor::EndDraw() const
{
    ImGui::Render();

    if (m_config.renderingBackend == GraphicsAPI::OpenGL)
    {
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
    else
    {
        // Not yet implemented.
        assert(false);
    }

    if (const ImGuiIO& io = ImGui::GetIO(); io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
        const SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
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

    if (menuItem.widget) m_widgets.push_back(menuItem.widget);
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
                Logger::Instance().Log(EditorLog, "TODO: Open a file creating dialogue");
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
                Logger::Instance().Log(EditorLog, "TODO: Open a file opening dialogue");
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
                Logger::Instance().Log(EditorLog, "TODO: Save a file");
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
                Logger::Instance().Log(EditorLog, "TODO: Quit app");
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
                Logger::Instance().Log(EditorLog, "TODO: Undo");
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
                Logger::Instance().Log(EditorLog, "TODO: Redo");
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
                Logger::Instance().Log(EditorLog, "TODO: Cut");
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
                Logger::Instance().Log(EditorLog, "TODO: Copy");
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
                Logger::Instance().Log(EditorLog, "TODO: Paste");
            }
        }
    };
    std::vector<MenuItem> toolsMenuItems = {
        {
            DEVELOPER_TOOLS,
            "Console",
            "Open a debug console",
            "",
            std::make_shared<Console>()
        },
        {
            DEVELOPER_TOOLS,
            "Profiler",
            "Open a profiler",
            "",
            std::make_shared<Profiler>()
        }
    };

    for (const auto menuItem : fileMenuItems)
    {
        AddMenuItem("File", menuItem);
    }
    for (const auto menuItem : editMenuItems)
    {
        AddMenuItem("Edit", menuItem);
    }
    for (const auto menuItem : toolsMenuItems)
    {
        AddMenuItem("Tools", menuItem);
    }
}

void Editor::DrawMainMenu(const RenderSystem& renderSystem)
{
    if (ImGui::BeginMainMenuBar()) {
        for (const auto& [menu, items] : m_menuItems)
        {
            if (ImGui::BeginMenu(menu.c_str()))
            {
                int group = -1;
                for (const auto& item : items)
                {
                    if (group != -1 && group != item.group)
                    {
                        ImGui::Separator(); // Adds a horizontal line separator
                    }

                    // ReSharper disable once CppTooWideScopeInitStatement
                    bool* p_visibility = item.widget ? &item.widget->visible : nullptr;
                    if (ImGui::MenuItem(item.name.c_str(), item.shortcut.c_str(), p_visibility))
                    {
                        if (item.handler) item.handler(item);
                    }
                    group = item.group;
                }
                ImGui::EndMenu();
            }
        }

        ImGui::EndMainMenuBar(); // End the main menu bar
    }
}
