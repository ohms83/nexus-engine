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

#include "Remotery.h"

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

    SetCullMode(PolygonFacing::Back);
    SetFrontFace(FrontFace::CounterClockWise);

    CALL_GL_FUNC(glEnable(GL_PROGRAM_POINT_SIZE));

    // Initialize Remotery
    rmt_BindOpenGL();
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
    {
        rmt_ScopedOpenGLSample(glClearColor);
        CALL_GL_FUNC(glClearColor(color.r, color.g, color.b, color.a));
    }
    {
        rmt_ScopedOpenGLSample(glClearDepth);
        CALL_GL_FUNC(glClearDepth(depth));
    }
    {
        rmt_ScopedOpenGLSample(glClearBuffer);
        CALL_GL_FUNC(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    }
}

void GLRenderingInterface::SwapBuffer()
{
    rmt_ScopedCPUSample(SDL_GL_SwapWindow, 0);
    SDL_GL_SwapWindow(m_window);
}

void GLRenderingInterface::SetViewport(int32_t x, int32_t y, int32_t w, int32_t h)
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

void GLRenderingInterface::DrawIndexed(const Ref<IndexBuffer> indexBuffer)
{
    rmt_ScopedCPUSample(GLRendering_DrawIndexed, 0);
    NXS_ASSERT(indexBuffer != nullptr);
    if (!indexBuffer->IsBinding()) indexBuffer->Bind();

    CALL_GL_FUNC(glFrontFace(GL::NxsFrontFaceToGL(indexBuffer->GetFrontFace())));

    // ReSharper disable once CppDFANullDereference
    const GLuint gl_drawMode = GL::NxsDrawModeToGL(indexBuffer->GetDrawMode());
    NXS_ASSERT_MSG(gl_drawMode != GL_QUADS, "GL_QUADS is not a valid primitive type.")
    
    {
        rmt_ScopedOpenGLSample(glDrawElements);
        CALL_GL_FUNC(glDrawElements(
            gl_drawMode,      // mode
            CAST<GLsizei>(indexBuffer->GetNumIndexDraw()),    // count
            GL_UNSIGNED_INT,   // type
            R_CAST<void*>(0)           // element array buffer offset
        ));
    }
}

void GLRenderingInterface::DrawIndexedInstanced(const Ref<IndexBuffer> indexBuffer, const uint32_t instanceCount)
{
    rmt_ScopedCPUSample(GLRendering_DrawIndexedInstanced, 0);
    NXS_ASSERT(indexBuffer != nullptr);
    if (!indexBuffer->IsBinding()) indexBuffer->Bind();

    CALL_GL_FUNC(glFrontFace(GL::NxsFrontFaceToGL(indexBuffer->GetFrontFace())));

    const GLuint gl_drawMode = GL::NxsDrawModeToGL(indexBuffer->GetDrawMode());
    NXS_ASSERT_MSG(gl_drawMode != GL_QUADS, "GL_QUADS is not a valid primitive type.")
    {
        rmt_ScopedOpenGLSample(glDrawElementsInstanced);
        CALL_GL_FUNC(glDrawElementsInstanced(
            gl_drawMode,
            CAST<GLsizei>(indexBuffer->GetNumIndexDraw()),
            GL_UNSIGNED_INT,
            R_CAST<void*>(0),
            CAST<GLsizei>(instanceCount)
        ));
    }
}

void GLRenderingInterface::OnResize(const uint32_t pixel_w, const uint32_t pixel_h)
{
    CALL_GL_FUNC(glViewport(0, 0, INT_CAST(pixel_w), INT_CAST(pixel_h)));
}

void GLRenderingInterface::SetColorMask(const glm::bvec4& mask)
{
    CALL_GL_FUNC(glColorMask((GLboolean)mask.r, (GLboolean)mask.g, (GLboolean)mask.b, (GLboolean)mask.a));
}

void GLRenderingInterface::SetDepthMask(bool mask)
{
    CALL_GL_FUNC(glDepthMask((GLboolean)mask));
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
    case DepthFunction::Equal:
        CALL_GL_FUNC(glDepthFunc(GL_EQUAL));
        break;
    case DepthFunction::Greater:
        CALL_GL_FUNC(glDepthFunc(GL_GREATER));
        break;
    case DepthFunction::Always:
        CALL_GL_FUNC(glDepthFunc(GL_ALWAYS));
        break;
    case DepthFunction::None:
        // Handled above.
        break;
    default:
        NXS_ASSERT(false);
        break;
    }
    m_depthFunction = depthFunction;
}

void GLRenderingInterface::SetLineWidth(float width)
{
    CALL_GL_FUNC(glLineWidth(width));
}

void GLRenderingInterface::SetPolygonMode(PolygonMode mode)
{
    Super::SetPolygonMode(mode);

    const auto gl_mode = GL::NxsPolygonModeToGL(mode);
    CALL_GL_FUNC(glPolygonMode(GL_FRONT_AND_BACK, gl_mode));
}

void GLRenderingInterface::SetCullMode(PolygonFacing mode)
{
    Super::SetCullMode(mode);

    if (mode == PolygonFacing::None)
    {
        CALL_GL_FUNC(glDisable(GL_CULL_FACE));
        return;
    }

    CALL_GL_FUNC(glEnable(GL_CULL_FACE));

    const auto gl_mode = GL::NxsFacingToGL(mode);
    CALL_GL_FUNC(glCullFace(gl_mode));
}

void GLRenderingInterface::SetFrontFace(FrontFace face)
{
    Super::SetFrontFace(face);
    const auto gl_face = GL::NxsFrontFaceToGL(face);
    CALL_GL_FUNC(glFrontFace(gl_face));
}

void GLRenderingInterface::EnableDrawBuffer(DrawBuffer buffer)
{
    switch (buffer)
    {
    case DrawBuffer::None:
        glDrawBuffer(GL_NONE);
        break;
    case DrawBuffer::Front:
        glDrawBuffer(GL_FRONT);
        break;
    case DrawBuffer::Back:
        glDrawBuffer(GL_BACK);
        break;
    default:
        NXS_ASSERT(false);
        break;
    }
}

void GLRenderingInterface::DebugMarker(const std::string& message)
{
    CALL_GL_FUNC(glDebugMessageInsert(
        GL_DEBUG_SOURCE_APPLICATION ,
        GL_DEBUG_TYPE_MARKER ,
        0,
        GL_DEBUG_SEVERITY_NOTIFICATION,
        message.length(),
        message.c_str()
    ));
}
