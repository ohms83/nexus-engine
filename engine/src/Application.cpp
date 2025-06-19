//
// Created by nutta on 6/18/2025.
//
#include "Application.h"

USING_NAMESPACE_NXS;

Application::Application()
{
}

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

    m_renderSystem = new RenderSystem(info.renderAPI);
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
        m_renderSystem->EndDraw();
    }
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
