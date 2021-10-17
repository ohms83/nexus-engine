#include "Application.hpp"

#include <sstream>

using namespace nexus;

Application::Application(const std::string& appName, uint32_t width, uint32_t height)
{
    _name = appName;
    _screenWidth = width;
    _screenHeight = height;

    if( SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        std::stringstream ss;
        ss << "Failed to initialize SDL. SDL_Error=" << SDL_GetError();
        throw std::runtime_error(ss.str());
    }

    _window = SDL_CreateWindow(appName.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, _screenWidth, _screenHeight, SDL_WINDOW_SHOWN );
    if( _window == NULL )
    {
        std::stringstream ss;
        ss << "Failed to create window. SDL_Error=" << SDL_GetError();
        throw std::runtime_error(ss.str());
    }


    //Get window surface
    _screenSurface = SDL_GetWindowSurface(_window);

    //Fill the surface white
    SDL_FillRect(_screenSurface, NULL, SDL_MapRGB(_screenSurface->format, 0x00, 0x00, 0x00));
    
    //Update the surface
    SDL_UpdateWindowSurface(_window);
}

Application::~Application()
{
    SDL_DestroyWindow(_window);
    SDL_Quit();
}

void Application::mainLoop()
{
    SDL_Event event;
    while(_running) {
        while(SDL_PollEvent(&event)) {
            onEvent(event);
        }
    }
}

void Application::onEvent(const SDL_Event& event)
{
    switch (event.type)
    {
    case SDL_QUIT:
        _running = false;
        break;
    default:
        break;
    }
}