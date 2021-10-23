#include "Application.hpp"

#include "base/SystemClock.hpp"

#include <sstream>
#include <iostream>

USING_NAMESPACE_NXS;

Application::Application()
{
}

Application::~Application()
{
    _renderSystem = nullptr;
    SDL_Quit();
}

void Application::init(const Info& info)
{
    SDL_version compiledVersion;
    SDL_version linkedVersion;

    SDL_VERSION(&compiledVersion);
    SDL_GetVersion(&linkedVersion);
    printf("SDL\n");
    printf("  Compiled Version : %d.%d.%d\n", compiledVersion.major, compiledVersion.minor, compiledVersion.patch);
    printf("  Linked Version   : %d.%d.%d\n", linkedVersion.major, linkedVersion.minor, linkedVersion.patch);

    if( SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        std::stringstream ss;
        ss << "Failed to initialize SDL. SDL_Error=" << SDL_GetError();
        throw std::runtime_error(ss.str());
    }

    _info = info;
    _renderSystem = std::unique_ptr<RenderSystem>(RenderSystem::create(info.renderSystem));
    if(_renderSystem == nullptr)
    {
        std::stringstream ss;
        ss << "Failed to create RenderSystem. API=" << info.renderSystem;
        throw std::runtime_error(ss.str());
    }
    _renderSystem->init(info.appName, info.screenWidth, info.screenHeight);
    _renderSystem->initGui();

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
            
            _renderSystem->beginDraw();
            _renderSystem->beginDrawGui();
            
            _renderSystem->draw();
            renderUI();
            
            _renderSystem->endDrawGui();
            _renderSystem->endDraw();
        }
    }
}

void Application::renderUI()
{
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

RenderSystem& Application::getRenderSystem()
{
    return *_renderSystem;
}
