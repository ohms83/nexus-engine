#include "Application.hpp"

#include "base/SystemClock.hpp"

#include <sstream>

USING_NAMESPACE_NXS;

Application::Application()
{
}

Application::~Application()
{
    SDL_DestroyWindow(_window);
    SDL_Quit();
}

void Application::init(const std::string& appName, uint32_t width, uint32_t height)
{
    _name = appName;
    _screenWidth = width;
    _screenHeight = height;

    _renderSystem = std::unique_ptr<RenderSystem>(createRenderSystem());

    if( SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        std::stringstream ss;
        ss << "Failed to initialize SDL. SDL_Error=" << SDL_GetError();
        throw std::runtime_error(ss.str());
    }

    _window = SDL_CreateWindow(_name.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, _screenWidth, _screenHeight, SDL_WINDOW_SHOWN );
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

    onInit();
}

void Application::mainLoop()
{
    SystemClock clock;
    clock.tick();

    SDL_Event event;
    while(_running) {
        float dt = (float)clock.getDiffMilliseconds() / 1000.0f;
        clock.tick();

        while(SDL_PollEvent(&event)) {
            // SDL_QUIT needs to be processed here to prevent child classes from ignoring it.
            if (event.type == SDL_QUIT) {
                _running = false;
            }
            onEvent(event);
            onUpdate(dt);
        }
    }
}

void Application::onInit()
{
    
}

void Application::onUpdate(float dt)
{

}

void Application::onEvent(const SDL_Event& event)
{
}