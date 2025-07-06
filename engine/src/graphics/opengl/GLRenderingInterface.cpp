//
// Created by nutta on 7/4/2025.
//

#include "graphics/opengl/GLRenderingInterface.h"

#include "graphics/opengl/GLIndexBuffer.h"
#include "graphics/opengl/GLVertexBuffer.h"

USING_NAMESPACE_NXS;

GLRenderingInterface::GLRenderingInterface(WindowContext window, const GraphicsConfig& config)
    : RenderingInterface()
{
    // TODO: Make it configurable.
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    m_window = window;
    m_renderContext = SDL_GL_CreateContext(window);
    if (!m_renderContext)
    {
        std::stringstream ss;
        ss << "Failed to created OpenGL's context! SDL_Error=" << SDL_GetError();
        throw std::runtime_error(ss.str());
    }

    if (!gladLoadGLLoader(R_CAST<GLADloadproc>(SDL_GL_GetProcAddress))) {
        std::stringstream ss;
        ss << "Failed to initialize GLAD" << std::endl;
        SDL_GL_DestroyContext(m_renderContext);
        SDL_DestroyWindow(window);
        SDL_Quit();
        throw std::runtime_error(ss.str());
    }

    SDL_GL_MakeCurrent(window, m_renderContext);

    std::stringstream ss;
    ss  << "OpenGL\n"
        << "  Version : " << glGetString(GL_VERSION) << "\n"
        << "  GLSL    : " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n"
        << "  Vendor  : " << glGetString(GL_VENDOR) << "\n"
        << "  Renderer: " << glGetString(GL_RENDERER);
    std::cout << ss.str() << std::endl;

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);
}

GLRenderingInterface::~GLRenderingInterface()
{
    SDL_GL_DestroyContext(m_renderContext);
}

void GLRenderingInterface::ClearColor(const Color4F& color)
{
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void GLRenderingInterface::ClearDepth(float depth)
{
    glClearDepth(depth);
    glClear(GL_DEPTH_BUFFER_BIT);
}

void GLRenderingInterface::ClearBuffer(const Color4F& color, float depth)
{
    glClearColor(color.r, color.g, color.b, color.a);
    glClearDepth(depth);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GLRenderingInterface::SwapBuffer()
{
    SDL_GL_SwapWindow(m_window);
}

VertexBuffer* GLRenderingInterface::CreateVertexBuffer()
{
    return new GLVertexBuffer();
}

IndexBuffer* GLRenderingInterface::CreateIndexBuffer(std::vector<uint32>&& indices, BufferUsage usage)
{
    return new GLIndexBuffer(std::move(indices), usage);
}

void GLRenderingInterface::OnResize(uint32_t pixel_w, uint32_t pixel_h)
{
    glViewport(0, 0, INT_CAST(pixel_w), INT_CAST(pixel_h));
}
