#include "GLRenderSystem.hpp"

#include <iostream>
#include <sstream>

USING_NAMESPACE_NXS;

GLRenderSystem::GLRenderSystem()
{

}

GLRenderSystem::~GLRenderSystem()
{
    SDL_GL_DeleteContext(_context);
}

void GLRenderSystem::init(SDL_Window* window)
{
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    _window = window;
    _context = SDL_GL_CreateContext(_window);

    std::stringstream ss;
    ss  << "OpenGL\n"
        << "  Version : " << glGetString(GL_VERSION) << "\n"
        << "  GLSL    : " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n"
        << "  Vendor  : " << glGetString(GL_VENDOR) << "\n"
        << "  Renderer: " << glGetString(GL_RENDERER);
    std::cout << ss.str() << std::endl;
}

int GLRenderSystem::getSDLInitFlag() const
{
    return SDL_WINDOW_OPENGL;
}

void GLRenderSystem::setClearColor(const Color& color)
{
    glClearColor(_clearColor.r, _clearColor.g, _clearColor.b, _clearColor.a);
}

void GLRenderSystem::clear()
{
    glClear(GL_COLOR_BUFFER_BIT);
}

void GLRenderSystem::draw()
{
}

void GLRenderSystem::swapBuffer()
{
    SDL_GL_SwapWindow(_window);
}