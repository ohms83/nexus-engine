//
// Created by nutta on 7/4/2025.
//

#include "graphics/opengl/GLRenderingInterface.h"

#include "graphics/opengl/GLIndexBuffer.h"
#include "graphics/opengl/GLShader.h"
#include "graphics/opengl/GLTexture.h"
#include "graphics/opengl/GLVertexBuffer.h"
#include "SDL3/SDL_error.h"

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
    const auto renderContext = SDL_GL_CreateContext(window);
    if (!renderContext)
    {
        std::stringstream ss;
        ss << "Failed to created OpenGL's context! SDL_Error=" << SDL_GetError();
        throw std::runtime_error(ss.str());
    }

    m_renderContext.gl_context = renderContext;

    if (!gladLoadGLLoader(R_CAST<GLADloadproc>(SDL_GL_GetProcAddress))) {
        std::stringstream ss;
        ss << "Failed to initialize GLAD" << std::endl;
        SDL_GL_DestroyContext(renderContext);
        SDL_DestroyWindow(window);
        SDL_Quit();
        throw std::runtime_error(ss.str());
    }

    SDL_GL_MakeCurrent(window, renderContext);

    std::stringstream ss;
    ss  << "OpenGL\n"
        << "  Version : " << glGetString(GL_VERSION) << "\n"
        << "  GLSL    : " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n"
        << "  Vendor  : " << glGetString(GL_VENDOR) << "\n"
        << "  Renderer: " << glGetString(GL_RENDERER);
    std::cout << ss.str() << std::endl;

    // Enable depth test
    CALL_GL_FUNC(glEnable(GL_DEPTH_TEST));
    // Accept fragment if it closer to the camera than the former one
    CALL_GL_FUNC(glDepthFunc(GL_LESS));
    
    CALL_GL_FUNC(glEnable(GL_CULL_FACE));
    CALL_GL_FUNC(glCullFace(GL_BACK));
    CALL_GL_FUNC(glFrontFace(GL_CW));
}

GLRenderingInterface::~GLRenderingInterface()
{
    SDL_GL_DestroyContext(m_renderContext.gl_context);
}

void GLRenderingInterface::ClearColor(const Color4F& color)
{
    CALL_GL_FUNC(glClearColor(color.r, color.g, color.b, color.a));
    CALL_GL_FUNC(glClear(GL_COLOR_BUFFER_BIT));
}

void GLRenderingInterface::ClearDepth(float depth)
{
    CALL_GL_FUNC(glClearDepth(depth));
    CALL_GL_FUNC(glClear(GL_DEPTH_BUFFER_BIT));
}

void GLRenderingInterface::ClearBuffer(const Color4F& color, float depth)
{
    CALL_GL_FUNC(glClearColor(color.r, color.g, color.b, color.a));
    CALL_GL_FUNC(glClearDepth(depth));
    CALL_GL_FUNC(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void GLRenderingInterface::SwapBuffer()
{
    SDL_GL_SwapWindow(m_window);
}

void GLRenderingInterface::SetViewport(int32 x, int32 y, int32 w, int32 h)
{
    CALL_GL_FUNC(glViewport(x, y, w, h));
}

VertexBuffer* GLRenderingInterface::CreateVertexBuffer() const
{
    return new GLVertexBuffer();
}

IndexBuffer* GLRenderingInterface::CreateIndexBuffer() const
{
    return new GLIndexBuffer();
}

Shader* GLRenderingInterface::CreateShader() const
{
    return new GLShader();
}

TextureProxy* GLRenderingInterface::CreateTexture() const
{
    return new GLTexture();
}

void GLRenderingInterface::Draw_Internal(const RenderCommand& command)
{
    const auto indexBuffer = command.indexBuffer;
    assert(indexBuffer != nullptr);

    GLuint gl_drawMode = 0;
    switch (indexBuffer->GetDrawMode())
    {
    case DrawMode::Point:
        gl_drawMode = GL_POINT;
        break;
    case DrawMode::Line:
        gl_drawMode = GL_LINE;
        break;
    case DrawMode::LineStrip:
        gl_drawMode = GL_LINE_STRIP;
        break;
    case DrawMode::LineLoop:
        gl_drawMode = GL_LINE_LOOP;
        break;
    case DrawMode::Triangle:
        gl_drawMode = GL_TRIANGLES;
        break;
    case DrawMode::TriangleStrip:
        gl_drawMode = GL_TRIANGLE_STRIP;
        break;
    case DrawMode::TriangleFan:
        gl_drawMode = GL_TRIANGLE_FAN;
        break;
    case DrawMode::Quad:
        gl_drawMode = GL_QUADS;
        break;
    default:
        assert(false);
        break;
    }
    CALL_GL_FUNC(glDrawElements(
         gl_drawMode,      // mode
         CAST<GLsizei>(command.indexBuffer->NumIndex()),    // count
         GL_UNSIGNED_INT,   // type
         R_CAST<void*>(0)           // element array buffer offset
     ));
}

void GLRenderingInterface::OnResize(uint32_t pixel_w, uint32_t pixel_h)
{
    CALL_GL_FUNC(glViewport(0, 0, INT_CAST(pixel_w), INT_CAST(pixel_h)));
}
