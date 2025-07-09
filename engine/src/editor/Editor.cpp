//
// Created by nutta on 7/9/2025.
//

#include <nexus/editor/Editor.h>
#include <nexus/graphics/RenderSystem.h>

// ImGui headers
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_opengl3.h>

USING_NAMESPACE_NXS;

Editor::Editor(const WindowContext window, const RenderContext renderContext, const EditorConfig& config)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;    // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;       // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;     // Enable Multi-Viewport / Platform Windows
    io.FontGlobalScale = 1.75f;
    io.ConfigDpiScaleFonts = true;                          // [Experimental] Automatically overwrite style.FontScaleDpi in Begin() when Monitor DPI changes. This will scale fonts but _NOT_ scale sizes/padding for now.
    io.ConfigDpiScaleViewports = true;                      // [Experimental] Scale Dear ImGui and Platform Windows when Monitor DPI changes.

    // Setup ImGui style
    ImGui::StyleColorsDark();
    // When viewports are enabled, tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    // --- Make windows transparent ---
    style.Colors[ImGuiCol_WindowBg].w = 0.65f;

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 0.65f;
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
}

Editor::~Editor()
{
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
}

void Editor::Draw(const RenderSystem& renderSystem) const
{
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    bool mainWindowActive = false;
    ImGui::Begin("Profiler", &mainWindowActive);
    {
        const auto& api = GraphicsAPIToString(m_config.renderingBackend);
        ImGui::Text("Graphics API: %s", api.c_str());
        ImGui::Text("FPS: %.2f", io.Framerate);
        ImGui::Text("Game Thread: %.2f ms", 1000.0f / io.Framerate);
        ImGui::Text("Render Thread: %.2f ms", renderSystem.GetrenderTime() * 1000.f);
        ImGui::Text("Draw Calls: %d", renderSystem.GetDrawCount());
        ImGui::Text("Polygons: %d", renderSystem.GetPolygonCount());
    }
    ImGui::End();

    // Example of how to implement a toolbar.
    // ImGui::Begin("Profiler", &mainWindowActive, ImGuiWindowFlags_MenuBar);
    // {
    //     if (ImGui::BeginMenuBar())
    //     {
    //         if (ImGui::BeginMenu("File"))
    //         {
    //             if (ImGui::MenuItem("Open..", "Ctrl+O")) { /* Do stuff */ }
    //             if (ImGui::MenuItem("Save", "Ctrl+S"))   { /* Do stuff */ }
    //             if (ImGui::MenuItem("Close", "Ctrl+W"))  { /* Do stuff */ }
    //             ImGui::EndMenu();
    //         }
    //         ImGui::EndMenuBar();
    //     }
    // }
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

    if (const ImGuiIO& io = ImGui::GetIO(); io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
        const SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
    }
}
