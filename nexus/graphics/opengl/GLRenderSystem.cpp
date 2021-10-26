#include "GLRenderSystem.hpp"

#include <iostream>
#include <sstream>

#include "GLVertexBuffer.hpp"
#include "GLShader.hpp"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

USING_NAMESPACE_NXS;

GLRenderSystem::GLRenderSystem()
{

}

GLRenderSystem::~GLRenderSystem()
{
    SDL_GL_DeleteContext(_renderContext);
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

    _renderContext = SDL_GL_CreateContext(_window);
    if (!_renderContext)
    {
        std::stringstream ss;
        ss << "Failed to created OpenGL's context! SDL_Error=" << SDL_GetError();
        throw std::runtime_error(ss.str());
    }
    SDL_GL_MakeCurrent(_window, _renderContext);

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

void GLRenderSystem::initGui()
{
    RenderSystem::initGui();
    
    // Setup Platform/Renderer backends
    const char* glslVersion = "#version 150";
    ImGui_ImplSDL2_InitForOpenGL(getWindow(), (SDL_GLContext)getRenderContext());
    ImGui_ImplOpenGL3_Init(glslVersion);
}

void GLRenderSystem::beginDrawGui()
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    
    RenderSystem::beginDrawGui();
}

void GLRenderSystem::endDrawGui()
{
    RenderSystem::endDrawGui();
    
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

VertexBuffer* GLRenderSystem::createVertexBuffer()
{
    return new GLVertexBuffer();
}

Shader* GLRenderSystem::createShader()
{
    return new GLShader();
}

void GLRenderSystem::setClearColor(const Color4F& color)
{
    RenderSystem::setClearColor(color);
    glClearColor(color.r, color.g, color.b, color.a);
}

void GLRenderSystem::clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GLRenderSystem::swapBuffer()
{
    SDL_GL_SwapWindow(_window);
}

void GLRenderSystem::useShader(std::shared_ptr<const Shader> shader) const
{
    glUseProgram(shader->getProgramId());
    CHECK_GL_ERROR();
}

void GLRenderSystem::execute(std::shared_ptr<const RenderCommand> command) const
{
    for (const auto& keyValue : command->getVec2Uniforms()) {
        const glm::vec2& value = keyValue.second;
        glUniform2f(keyValue.first, value.x, value.y);
        CHECK_GL_ERROR();
    }
    
    for (const auto& keyValue : command->getVec3Uniforms()) {
        const glm::vec3& value = keyValue.second;
        glUniform3fv(keyValue.first, 1, (float*)&value);
        CHECK_GL_ERROR();
    }
    
    for (const auto& keyValue : command->getVec4Uniforms()) {
        const glm::vec4& value = keyValue.second;
        glUniform4fv(keyValue.first, 1, (float*)&value);
        CHECK_GL_ERROR();
    }
    
    for (const auto& keyValue : command->getMat3Uniforms()) {
        const glm::mat3& value = keyValue.second;
        glUniformMatrix3fv(keyValue.first, 1, GL_FALSE, (float*)&value);
        CHECK_GL_ERROR();
    }
    
    for (const auto& keyValue : command->getMat4Uniforms()) {
        const glm::mat4& value = keyValue.second;
        glUniformMatrix4fv(keyValue.first, 1, GL_FALSE, (float*)&value);
        CHECK_GL_ERROR();
    }
    
    for (const auto& vertexBuffer : command->getBufferList())
    {
        const GLVertexBuffer* glVertexBuffer = dynamic_cast<const GLVertexBuffer*>(vertexBuffer.get());
        if (!glVertexBuffer) {
            continue;
        }
        
        const VertexBufferInfo& bufferInfo = glVertexBuffer->getInfo();
        
        glBindVertexArray(glVertexBuffer->getVAO());
        CHECK_GL_ERROR();
        
        if (bufferInfo.indexCount > 0) {
            glDrawElements(bufferInfo.primitiveType, bufferInfo.indexCount, GL_UNSIGNED_INT, 0);
        }
        else {
            glDrawArrays(bufferInfo.primitiveType, 0, bufferInfo.vertexCount);
        }
        CHECK_GL_ERROR();
    }
}
