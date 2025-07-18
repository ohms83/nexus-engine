//
// Created by nutta on 6/18/2025.
//
#include "Application.h"

#include <iostream>
#include <ostream>

#include "resource/Texture.h"

#if defined(SDL_PLATFORM_WIN32)
#include <windows.h>
#endif

USING_NAMESPACE_NXS;

Application::~Application()
{
    TextureManager::GetInstance().PurgeUnused();

    delete m_renderSystem;
    m_renderSystem = nullptr;

    m_editor.reset();

    //Destroy the window
    SDL_DestroyWindow(m_window);
    m_window = nullptr;

    SDL_Quit();
}

bool Application::Init(const ApplicationConfig& info)
{
    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) == false )
    {
        SDL_Log( "SDL could not initialize! SDL error: %s\n", SDL_GetError() );
        return false;
    }

    int flags = info.fullscreen ? SDL_WINDOW_FULLSCREEN : 0;
    flags |= info.resizable ? SDL_WINDOW_RESIZABLE : 0;

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
        SDL_Log("Window could not be created. SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    SDL_GetWindowSizeInPixels(m_window, &m_actualSize.x, &m_actualSize.y);
    std::cout << "Actual window size width: " << m_actualSize.x << " height: " << m_actualSize.y << std::endl;

    m_renderSystem = new RenderSystem(m_window, graphicsConfig);
    assert(m_renderSystem);

    if (info.editMode)
    {
        EditorConfig editorConfig {
            info.graphicsConfig.api
        };
        RenderContext renderContext = m_renderSystem->GetRenderContext();
        m_editor = std::make_unique<Editor>(m_window, renderContext, editorConfig);
    }

    return Init_Internal();
}

int Application::BeginMainLoop()
{
    m_timer.Stamp();

    //The event data
    SDL_Event e;
    SDL_zero(e);

    while(!m_quit)
    {
        m_deltaTime = m_timer.GetDeltaTime();
        m_timer.Stamp();

        PollEvents(e);
        Update();

        if (m_editor)
        {
            m_editor->BeginDraw();
            m_editor->Draw(*m_renderSystem);

            UpdateEditor();
            RenderEditor(*m_renderSystem);
        }

        m_renderSystem->BeginDraw();
        Render(*m_renderSystem);

        m_renderSystem->Draw();

        if (m_editor) m_editor->EndDraw();
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

WindowContext Application::GetWindowContext() const
{
    return m_window;
}

void Application::OnKeyDown(const SDL_Keycode key)
{
    if (key == m_escapeKey)
    {
        RequestQuit();
    }
}

void Application::OnResize(const glm::ivec2& screenSize, const glm::ivec2& actualSize)
{
    m_renderSystem->OnResize(actualSize.x, actualSize.y);
}

void Application::PollEvents(SDL_Event& e)
{
    //Get event data
    while(SDL_PollEvent(&e))
    {
        if (m_editor) m_editor->Update(e);

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
}
