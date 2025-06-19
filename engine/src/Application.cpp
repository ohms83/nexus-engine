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
    //Destroy the window
    SDL_DestroyWindow(m_window);
    m_window = nullptr;

    SDL_Quit();
}

bool Application::Init()
{
    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) == false )
    {
        SDL_Log( "SDL could not initialize! SDL error: %s\n", SDL_GetError() );
        return false;
    }

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
