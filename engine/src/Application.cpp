//
// Created by nutta on 6/18/2025.
//
#include "Application.h"

#include <iostream>
#include <ostream>

#if defined(SDL_PLATFORM_WIN32)
#include <windows.h>
#endif

USING_NAMESPACE_NXS;

Application::~Application()
{
    delete m_renderSystem;
    m_renderSystem = nullptr;

    //Destroy the window
    SDL_DestroyWindow(m_window);
    m_window = nullptr;

    SDL_Quit();
}

bool Application::Init(const ApplicationInitInfo& info)
{
    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) == false )
    {
        SDL_Log( "SDL could not initialize! SDL error: %s\n", SDL_GetError() );
        return false;
    }

    int flags = info.fullscreen ? SDL_WINDOW_FULLSCREEN : 0;
    switch (info.renderAPI)
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

    m_screenWidth = info.screenWidth;
    m_screenHeight = info.screenHeight;
    m_escapeKey = info.quitKey;
    m_window = SDL_CreateWindow(
        info.title.c_str(), m_screenWidth, m_screenHeight, flags);

    if (!m_window) {
        SDL_Log("Window could not be created. SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    SDL_GetWindowSizeInPixels(m_window, &m_actualWidth, &m_actualHeight);
    std::cout << "Actual window size width: " << m_actualWidth << " height: " << m_actualHeight << std::endl;

    RenderSystemConfig renderConfig{};
#if defined(SDL_PLATFORM_WIN32)
    HWND hwnd = nullptr;
    hwnd = CAST<HWND>(
        SDL_GetPointerProperty(SDL_GetWindowProperties(m_window),
            SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr));
    renderConfig.windowHandle = hwnd;
#else
    // No implementation.
    assert(false);
#endif

    renderConfig.screenWidth = m_actualWidth;
    renderConfig.screenHeight = m_actualHeight;
    renderConfig.api = info.renderAPI;
    renderConfig.vsync = false;
#ifdef _DEBUG
    renderConfig.debugFlags = RenderDebugFlags::NoIFS;
#else
    renderConfig.debugFlags = RenderDebugFlags::None;
#endif
    m_renderSystem = new RenderSystem(renderConfig);
    assert(m_renderSystem);

    return Init_Internal();
}

int Application::BeginMainLoop()
{
    //The event data
    SDL_Event e;
    SDL_zero(e);

    while(!m_quit)
    {
        PollEvents(e);
        Update();
        m_renderSystem->ClearScreen();
        m_renderSystem->BeginDraw();
        Render(m_renderSystem);
        m_renderSystem->Draw();
        m_renderSystem->EndDraw();    }
    return 0;
}

void Application::RequestQuit()
{
    m_quit = true;
}

bool Application::IsQuitRequested() const
{
    return m_quit;
}

void Application::OnKeyDown(const SDL_Keycode key)
{
    if (key == m_escapeKey)
    {
        RequestQuit();
    }
}

void Application::OnResize()
{
    int new_width, new_height;
    SDL_GetWindowSizeInPixels(m_window, &new_width, &new_height);

    m_renderSystem->OnResize(new_width, new_height);
}

void Application::PollEvents(SDL_Event& e)
{
    //Get event data
    while(SDL_PollEvent(&e))
    {
        switch (e.type)
        {
        case SDL_EVENT_KEY_DOWN:
            OnKeyDown(e.key.key);
            break;
        case SDL_EVENT_QUIT:
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            //End the main loop
            RequestQuit();
            break;
        case SDL_EVENT_WINDOW_METAL_VIEW_RESIZED:
        case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
        case SDL_EVENT_WINDOW_RESIZED:
            OnResize();
            break;
        default:
            break;
        }
    }
}
