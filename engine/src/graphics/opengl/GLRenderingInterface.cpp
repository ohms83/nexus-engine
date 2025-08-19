//
// Created by nutta on 7/4/2025.
//

#include "graphics/opengl/GLRenderingInterface.h"

#include <sstream>

#include "core/Logger.h"
#include "graphics/opengl/GLIndexBuffer.h"
#include "graphics/opengl/GLGpuProgram.h"
#include "graphics/opengl/GLTexture.h"
#include "graphics/opengl/GLVertexBuffer.h"
#include "SDL3/SDL_error.h"

USING_NAMESPACE_NXS;

GLRenderingInterface::GLRenderingInterface(WindowContext window, const GraphicsConfig& config)
    : RenderingInterface()
{
    m_graphicsAPI = GraphicsAPI::OpenGL;
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
        LOG_FATAL(LogOpenGL, std::format("Failed to created OpenGL's context! SDL_Error={}", SDL_GetError()));
        return;
    }

    m_renderContext.gl_context = renderContext;

    if (!gladLoadGLLoader(R_CAST<GLADloadproc>(SDL_GL_GetProcAddress)))
    {
        LOG_FATAL(LogOpenGL, "Failed to initialize GLAD");
        return;
    }

    SDL_GL_MakeCurrent(window, renderContext);

    std::stringstream ss;
    ss  << "OpenGL\n"
        << "  Version : " << glGetString(GL_VERSION) << "\n"
        << "  GLSL    : " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n"
        << "  Vendor  : " << glGetString(GL_VENDOR) << "\n"
        << "  Renderer: " << glGetString(GL_RENDERER);
    LOG_INFO(LogOpenGL, ss.str());

    // Enable depth test
    SetDepthFunction(DepthFunction::Lesser);

    CALL_GL_FUNC(glEnable(GL_CULL_FACE));
    CALL_GL_FUNC(glCullFace(GL_BACK));
    CALL_GL_FUNC(glFrontFace(GL_CCW));

    CALL_GL_FUNC(glEnable(GL_PROGRAM_POINT_SIZE));
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

GpuProgram* GLRenderingInterface::CreateGpuProgram() const
{
    return new GLGpuProgram();
}

TextureProxy* GLRenderingInterface::CreateTexture() const
{
    return new GLTexture();
}

void GLRenderingInterface::Draw_Internal(const RenderCommand& command)
{
    const auto indexBuffer = command.indexBuffer;
    NXS_ASSERT(indexBuffer != nullptr);

    CALL_GL_FUNC(glFrontFace(GL::NxsFrontFaceToGL(indexBuffer->GetFrontFace())));

    // ReSharper disable once CppDFANullDereference
    const GLuint gl_drawMode = GL::NxsDrawModeToGL(indexBuffer->GetDrawMode());
    NXS_ASSERT_MSG(gl_drawMode != GL_QUADS, "GL_QUADS is not a valid primitive type.")
    CALL_GL_FUNC(glDrawElements(
         gl_drawMode,      // mode
         CAST<GLsizei>(command.indexBuffer->NumIndex()),    // count
         GL_UNSIGNED_INT,   // type
         R_CAST<void*>(0)           // element array buffer offset
     ));
}

void GLRenderingInterface::OnResize(const uint32_t pixel_w, const uint32_t pixel_h)
{
    CALL_GL_FUNC(glViewport(0, 0, INT_CAST(pixel_w), INT_CAST(pixel_h)));
}

void GLRenderingInterface::SetDepthFunction(const DepthFunction depthFunction)
{
    if (m_depthFunction == depthFunction) return;

    if (depthFunction == DepthFunction::None)
    {
        CALL_GL_FUNC(glDisable(GL_DEPTH_TEST));
    }
    else
    {
        CALL_GL_FUNC(glEnable(GL_DEPTH_TEST));
    }

    switch (depthFunction)
    {
    case DepthFunction::Lesser:
        CALL_GL_FUNC(glDepthFunc(GL_LESS));
        break;
    case DepthFunction::LesserOrEqual:
        CALL_GL_FUNC(glDepthFunc(GL_LEQUAL));
        break;
    case DepthFunction::Greater:
        CALL_GL_FUNC(glDepthFunc(GL_GREATER));
        break;
    case DepthFunction::Always:
        CALL_GL_FUNC(glDepthFunc(GL_ALWAYS));
        break;
    default:
        NXS_ASSERT(false);
        break;
    }
    m_depthFunction = depthFunction;
}
