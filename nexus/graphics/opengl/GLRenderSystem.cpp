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
    _window = window;
    _context = SDL_GL_CreateContext(_window);
    if (!_context) {
        throw std::runtime_error("Failed to initialize OpenGL's context!");
    }

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