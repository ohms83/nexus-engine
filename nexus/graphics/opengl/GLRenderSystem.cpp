#include "GLRenderSystem.hpp"

#include <iostream>
#include <sstream>

#include "GLVertexBuffer.hpp"
#include "GLShader.hpp"
#include "GLRenderCommand.hpp"

USING_NAMESPACE_NXS;

GLRenderSystem::GLRenderSystem()
{

}

GLRenderSystem::~GLRenderSystem()
{
    SDL_GL_DeleteContext(_context);
    SDL_DestroyWindow(_window);
}

void GLRenderSystem::init(std::string appName, uint32_t screenWidth, uint32_t screenHeight)
{
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    
#if defined(__APPLE__)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
#endif

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    int flags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;

    _window = SDL_CreateWindow(appName.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, flags );
    if( _window == NULL )
    {
        std::stringstream ss;
        ss << "Failed to create window! SDL_Error=" << SDL_GetError();
        throw std::runtime_error(ss.str());
    }

    _context = SDL_GL_CreateContext(_window);
    if (!_context)
    {
        std::stringstream ss;
        ss << "Failed to created OpenGL's context! SDL_Error=" << SDL_GetError();
        throw std::runtime_error(ss.str());
    }

    std::stringstream ss;
    ss  << "OpenGL\n"
        << "  Version : " << glGetString(GL_VERSION) << "\n"
        << "  GLSL    : " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n"
        << "  Vendor  : " << glGetString(GL_VENDOR) << "\n"
        << "  Renderer: " << glGetString(GL_RENDERER);
    std::cout << ss.str() << std::endl;
}

VertexBuffer* GLRenderSystem::createVertexBuffer()
{
    return new GLVertexBuffer();
}

Shader* GLRenderSystem::createShader()
{
    return new GLShader();
}

RenderCommand* GLRenderSystem::createCommand()
{
    return new GLRenderCommand();
}

void GLRenderSystem::setClearColor(const Color4F& color)
{
    glClearColor(_clearColor.r, _clearColor.g, _clearColor.b, _clearColor.a);
}

void GLRenderSystem::clear()
{
    glClear(GL_COLOR_BUFFER_BIT);
}

void GLRenderSystem::swapBuffer()
{
    SDL_GL_SwapWindow(_window);
}
