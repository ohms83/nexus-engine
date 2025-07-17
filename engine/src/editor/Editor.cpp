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

USING_NAMESPACE_NXS;

#define MAX_PROFILED_FRAMES 20

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

    m_frameTimes.reserve(MAX_PROFILED_FRAMES);
    m_frameCounters.reserve(MAX_PROFILED_FRAMES);

    for (int i = 0; i < MAX_PROFILED_FRAMES; i++)
    {
        m_frameTimes.push_back(0);
        m_frameCounters.push_back(0);
    }

    m_console = std::make_unique<Console>();
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
    DrawProfiler(renderSystem);
    m_console->Draw(renderSystem);
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

void Editor::DrawProfiler(const RenderSystem& renderSystem)
{
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    bool mainWindowActive = false;
    ImGui::Begin("Profiler", &mainWindowActive);
    {
        static uint32 frameIndex = 0;
        const auto frameTime = 1000.0f / io.Framerate; // Frame update time in seconds.
        const auto& api = GraphicsAPIToString(m_config.renderingBackend);
        ImGui::Text("Graphics API: %s", api.c_str());
        ImGui::Text("FPS: %.2f", io.Framerate);
        ImGui::Text("Frame Update: %.2f ms", frameTime);
        ImGui::Text("Render: %.2f ms", renderSystem.GetrenderTime() * 1000.f);
        ImGui::Text("Draw Calls: %d", renderSystem.GetDrawCount());
        ImGui::Text("Polygons: %d", renderSystem.GetPolygonCount());

        if (ImPlot::BeginPlot("Frame Rates"))
        {
            uint32 x_data[MAX_PROFILED_FRAMES] = {};
            uint32 y_data[MAX_PROFILED_FRAMES] = {};
            ImPlot::SetupAxes("Frame Index", "Milliseconds", ImPlotAxisFlags_NoTickLabels);
            ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.25f);
            ImPlot::SetupAxisLimits(ImAxis_X1,frameIndex > MAX_PROFILED_FRAMES ? frameIndex - MAX_PROFILED_FRAMES : 0, frameIndex++, ImGuiCond_Always);
            ImPlot::SetupAxisLimits(ImAxis_Y1,0,60);
            ImPlot::SetNextFillStyle(ImVec4(1,1,0,1),0.25f);
            m_frameCounters.push_back(renderSystem.GetFrameIndex());
            m_frameTimes.push_back(frameTime);

            if (m_frameCounters.size() > MAX_PROFILED_FRAMES) m_frameCounters.erase(m_frameCounters.begin());
            if (m_frameTimes.size() > MAX_PROFILED_FRAMES) m_frameTimes.erase(m_frameTimes.begin());

            ImPlot::PlotShaded("Frame", m_frameCounters.data(), m_frameTimes.data(), MAX_PROFILED_FRAMES);
            ImPlot::EndPlot();
        }
    }
    ImGui::End();
}

void Editor::DrawMainMenu(const RenderSystem& renderSystem)
{
    // In your main.cpp render loop, after ImGui::NewFrame()

    // (Optional) If you have a docking setup with PassthruCentralNode,
    // you might want to ensure the menu bar doesn't interfere,
    // or use ImGui::GetMainViewport()->Pos to offset if needed.
    // For a standalone app, just a plain BeginMainMenuBar is fine.

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New", "Ctrl+N")) {
                // Do something on "New" click
                // For example:
            }
            if (ImGui::MenuItem("Open...", "Ctrl+O")) {
                // Do something on "Open" click
            }
            if (ImGui::MenuItem("Save", "Ctrl+S")) {
                // Do something on "Save" click
            }
            ImGui::Separator(); // Adds a horizontal line separator
            if (ImGui::MenuItem("Quit", "Alt+F4")) {
                // Set your application's quit flag
                // For example: quit = true; (if `quit` is your main loop control variable)
            }
            ImGui::EndMenu(); // End "File" menu
        }

        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Undo", "Ctrl+Z", false, false)) { // disabled, not greyed out
                // Do something on "Undo" click
            }
            if (ImGui::MenuItem("Redo", "Ctrl+Y", false, false)) { // disabled, not greyed out
                // Do something on "Redo" click
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "Ctrl+X")) {}
            if (ImGui::MenuItem("Copy", "Ctrl+C")) {}
            if (ImGui::MenuItem("Paste", "Ctrl+V")) {}
            ImGui::EndMenu(); // End "Edit" menu
        }

        if (ImGui::BeginMenu("View")) {
            // Example: toggle visibility of your debug console
            static bool show_console_menu_item = true; // Make sure this matches your actual console visibility flag
            if (ImGui::MenuItem("Console", "", &show_console_menu_item)) {
                // TODO:
            }
            ImGui::EndMenu(); // End "View" menu
        }

        ImGui::EndMainMenuBar(); // End the main menu bar
    }
}
