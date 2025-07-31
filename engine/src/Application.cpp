//
// Created by nutta on 6/18/2025.
//
#include "Application.h"

#include <format>
#include <iostream>
#include <ostream>

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl3.h"
#include "implot.h"
#include "core/Logger.h"
#include "core/TaskManager.h"
#include "../include/nexus/time/TimerManager.h"
#include "graphics/debug/Gizmos.h"
#include "io/InputManager.h"
#include "resource/Mesh.h"
#include "resource/Texture.h"

USING_NAMESPACE_NXS;

DEFINE_LOG(Application);

Application::~Application()
{
    auto& logger = Logger::Instance();
    // Shouldn't send out any callback at this point.
    logger.Disconnect();
    logger.Info(LogApplication, "Shutting down...");

    DestroyImGui();

    InputManager::Destroy();
    TimerManager::Destroy();
    TaskManager::Destroy();

    m_currentScene.reset();

    Gizmos::Cleanup();

    PURGE_UNUSED_RESOURCES(TextureManager);
    PURGE_UNUSED_RESOURCES(MeshManager);
    m_textureManager.reset();
    m_meshManager.reset();

    m_editor.reset();
    m_renderSystem.reset();

    //Destroy the window
    SDL_DestroyWindow(m_window);
    m_window = nullptr;

    SDL_Quit();

    logger.Info(LogApplication, "Shut down");
}

bool Application::Init(const ApplicationConfig& info)
{
    // TaskManager should be initialized before anything.
    TaskManager::Init();
    auto& taskManager = TaskManager::Instance();

    Logger::Init(Logger::LogToFile | Logger::LogToStdOut);
    auto& logger = Logger::Instance();
    
    // TODO: Move to another class
    // Flush the logs every second.
    taskManager.CreateTask(
        // Flush action
        [] { Logger::Instance().Flush(); },
        // Repeat counts
        -1,
        // Delay
        0,
        // Task repeat interval
        1,
        // Run immediately
        true);

    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) == false )
    {
        LOG_FATAL(LogApplication, std::format("SDL could not initialize! SDL error: {}\n", SDL_GetError()));
        return false;
    }

    int flags = info.fullscreen ? SDL_WINDOW_FULLSCREEN : 0;
    flags |= info.resizable ? SDL_WINDOW_RESIZABLE : 0;
    flags |= info.maximize ? SDL_WINDOW_MAXIMIZED : 0;

    const auto& graphicsConfig = info.graphicsConfig;
    switch (graphicsConfig.api)
    {
    case GraphicsAPI::OpenGL:
        flags |= SDL_WINDOW_OPENGL;
        break;
    case GraphicsAPI::Vulkan:
        flags |= SDL_WINDOW_VULKAN;
        break;
    default:
        break;
    }

    flags |= SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS;

    m_screenSize.x = graphicsConfig.screenWidth;
    m_screenSize.y = graphicsConfig.screenHeight;
    m_escapeKey = info.quitKey;
    m_window = SDL_CreateWindow(
        info.title.c_str(), m_screenSize.x, m_screenSize.y, flags);

    if (!m_window) {
        LOG_FATAL(LogApplication, std::format("Window could not be created. SDL_Error: {}\n", SDL_GetError()));
        return false;
    }

    SDL_GetWindowSizeInPixels(m_window, &m_actualSize.x, &m_actualSize.y);
    const auto log = std::format("Actual window size width: {} height: {}", m_actualSize.x, m_actualSize.y);
    logger.Info(LogApplication, log);

    m_renderSystem = std::make_unique<RenderSystem>(m_window, graphicsConfig);
    NXS_ASSERT(m_renderSystem);

    if (info.editMode)
    {
        m_editor = std::make_unique<Editor>();
    }

    m_meshManager = std::make_unique<MeshManager>();
    m_textureManager = std::make_unique<TextureManager>();

    Gizmos::Init(*m_renderSystem);
    InputManager::Init();
    TimerManager::Init();

    InitImGui();

    return Init_Internal();
}

int Application::BeginMainLoop()
{
    m_timer = TimerManager::Instance().GetTimer();

    //The event data
    SDL_Event e;
    SDL_zero(e);

    auto& inputManager = InputManager::Instance();
    auto& timerManager = TimerManager::Instance();
    auto& taskManager = TaskManager::Instance();

    while(!m_quit)
    {
        timerManager.Tick();
        m_deltaTime = m_timer->GetDeltaTime();

        PollEvents(e);

        Update();
        taskManager.Update();

        // Update input
        inputManager.Update();

        if (m_editor) m_editor->Update();

        m_renderSystem->BeginDraw();
        BeginDrawUI();

        // Render UIs
        DrawUI();
        if (m_editor) m_editor->Draw(*m_renderSystem);

        // Render the scene
        Render(*m_renderSystem);
        m_renderSystem->Draw();

        EndDrawUI();
        m_renderSystem->EndDraw();
    }
    return 0;
}

void Application::RequestQuit()
{
    // Check whether the application has already begun to quit.
    if (m_quit) return;

    LOG_INFO(LogApplication, "RequestQuit");
    m_quit = true;
}

bool Application::IsQuitRequested() const
{
    return m_quit;
}

WindowContext Application::GetWindowContext() const
{
    return m_window;
}

Ref<Scene> Application::ChangeScene(const Ref<Scene>& scene)
{
    return m_currentScene = scene;
}

Ref<Scene> Application::GetCurrentScene() const
{
    return m_currentScene;
}

void Application::Update()
{
    if (m_currentScene)
    {
        m_currentScene->Update(GetDeltaTime());
    }
}

void Application::Render(RenderSystem& renderSystem)
{
    if (m_currentScene)
    {
        m_currentScene->Render(renderSystem);
    }
}

void Application::OnEvent(const SDL_Event& e)
{
    ImGui_ImplSDL3_ProcessEvent(&e);

    switch (e.type)
    {
    case SDL_EVENT_WINDOW_FOCUS_LOST:
    case SDL_EVENT_KEY_DOWN:
        OnKeyDown(e.key.key);
        break;
    case SDL_EVENT_KEY_UP:
        OnKeyUp(e.key.key);
        break;
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
        OnMouseDown(e.button.button, e.button.x, e.button.y);
        break;
    case SDL_EVENT_MOUSE_BUTTON_UP:
        OnMouseUp(e.button.button, e.button.x, e.button.y);
        break;
    case SDL_EVENT_MOUSE_MOTION:
        // OnMouseMove(e.motion.xrel, e.motion.yrel);
        OnMouseMove(e.motion.x, e.motion.y);
        break;
    case SDL_EVENT_WINDOW_METAL_VIEW_RESIZED:
    case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
    case SDL_EVENT_WINDOW_RESIZED:
        {
            SDL_GetWindowSize(m_window, &m_screenSize.x, &m_screenSize.y);
            SDL_GetWindowSizeInPixels(m_window, &m_actualSize.x, &m_actualSize.y);
            OnResize(m_screenSize, m_actualSize);
        }
        break;
    default:
        break;
    }
}

void Application::OnFocusLost()
{
}

void Application::OnFocusGain()
{
}

void Application::OnKeyDown(const SDL_Keycode key)
{
    if (key == m_escapeKey)
    {
        RequestQuit();
    }

    if (ImGui::GetIO().WantCaptureKeyboard) return;
    InputManager::Instance().OnKeyDown(key);
}

void Application::OnKeyUp(const SDL_Keycode key)
{
    if (ImGui::GetIO().WantCaptureKeyboard) return;
    InputManager::Instance().OnKeyUp(key);
}

void Application::OnMouseDown(const int32 buttonId, const float x, const float y)
{
    if (ImGui::GetIO().WantCaptureMouse) return;
    InputManager::Instance().OnMouseDown(buttonId, x, y);
}

void Application::OnMouseUp(const int32 buttonId, const float x, const float y)
{
    if (ImGui::GetIO().WantCaptureMouse) return;
    InputManager::Instance().OnMouseUp(buttonId, x, y);
}

void Application::OnMouseMove(const float x, const float y)
{
    if (ImGui::GetIO().WantCaptureMouse) return;
    InputManager::Instance().OnMouseMove(x, y);
}

void Application::OnResize(const glm::ivec2& screenSize, const glm::ivec2& actualSize)
{
    LOG_INFO(LogApplication, std::format("OnResize screenSize: {}x{} actualSize: {}x{}", screenSize.x, screenSize.y, actualSize.x, actualSize.y));
    m_renderSystem->OnResize(actualSize.x, actualSize.y);
}

void Application::PollEvents(SDL_Event& e)
{
    //Get event data
    while(SDL_PollEvent(&e))
    {
        if (e.type == SDL_EVENT_QUIT || e.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED)
        {
            RequestQuit();
            break;
        }

        OnEvent(e);
    }
}

void Application::InitImGui() const
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
    style.Colors[ImGuiCol_WindowBg].w = 0.5f;

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 0.5f;
    }

    // You might also want to adjust the title bar background if it's too opaque
    style.Colors[ImGuiCol_TitleBg].w = 0.8f;
    style.Colors[ImGuiCol_TitleBgActive].w = 0.85f; // Active title bar
    style.Colors[ImGuiCol_TitleBgCollapsed].w =.7f; // Collapsed title bar

    if (const auto api = RenderingInterface::Instance().GetAPI(); api == GraphicsAPI::OpenGL)
    {
        // Setup Platform/Renderer backends
        ImGui_ImplSDL3_InitForOpenGL(m_window, m_renderSystem->GetRenderContext().gl_context);
        ImGui_ImplOpenGL3_Init(nullptr); // Match your core profile version
    }
    else
    {
        // Not yet implemented
        NXS_ASSERT_MSG(false, std::format("Unimplemented graphics API: {}", GraphicsAPIToString(api)));
    }

    // Initializes ImPlot
    ImPlot::CreateContext();
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void Application::BeginDrawUI()
{
    if (const auto api = RenderingInterface::Instance().GetAPI(); api == GraphicsAPI::OpenGL)
    {
        ImGui_ImplOpenGL3_NewFrame();
    }
    else
    {
        // Not yet implemented
        NXS_ASSERT_MSG(false, std::format("Unimplemented graphics API: {}", GraphicsAPIToString(api)));
    }
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
}

void Application::EndDrawUI()
{
    ImGui::Render();

    if (const auto api = RenderingInterface::Instance().GetAPI(); api == GraphicsAPI::OpenGL)
    {
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
    else
    {
        // Not yet implemented
        NXS_ASSERT_MSG(false, std::format("Unimplemented graphics API: {}", GraphicsAPIToString(api)));
    }

    if (const ImGuiIO& io = ImGui::GetIO(); io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
        SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
    }
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void Application::DestroyImGui()
{
    ImPlot::DestroyContext();

    if (const auto api = RenderingInterface::Instance().GetAPI(); api == GraphicsAPI::OpenGL)
    {
        ImGui_ImplOpenGL3_Shutdown();
    }

    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
}
