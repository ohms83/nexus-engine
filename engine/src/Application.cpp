//
// Created by nutta on 6/18/2025.
//
#include "Application.h"

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

    m_screenWidth = info.screenWidth;
    m_screenHeight = info.screenHeight;
    SDL_Window* m_window = SDL_CreateWindow(
        info.title.c_str(), m_screenWidth, m_screenHeight, SDL_WINDOW_OPENGL);

    if (!m_window) {
        SDL_Log("Window could not be created. SDL_Error: %s\n", SDL_GetError());
        return false;
    }

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

    renderConfig.screenWidth = m_screenWidth;
    renderConfig.screenHeight = m_screenHeight;
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

void Application::PollEvents(SDL_Event& e)
{
    //Get event data
    while(SDL_PollEvent(&e))
    {
        //If event is quit type
        if(e.type == SDL_EVENT_QUIT)
        {
            //End the main loop
            RequestQuit();
        }
    }
}
