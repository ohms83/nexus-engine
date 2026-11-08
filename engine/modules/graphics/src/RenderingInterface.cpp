//
// Created by nutta on 7/4/2025.
//
#include <array>
#include <functional>

#include "nexus/graphics/RenderingInterface.h"
#include "nexus/graphics/opengl/GLRenderingInterface.h"
#include "nexus/debug/Logger.h"

#include "Remotery.h"

USING_NAMESPACE_NXS;

DEFINE_LOG(RenderingInterface);

Ref<RenderingInterface> RenderingInterface::Create(WindowContext window, const GraphicsConfig& config)
{
    LOG_INFO(LogRenderingInterface, std::format("Creating Rendering Interface API: {}", GraphicsAPIToString(config.api)));
    Ref<RenderingInterface> result;
    switch (config.api)
    {
    case GraphicsAPI::OpenGL:
        result.reset(new GLRenderingInterface(window, config));
        break;
    case GraphicsAPI::Undefined:
        // Automatically choose the most suitable API based on the current platform.
#ifdef NXS_PLATFORM_WINDOWS
        // TODO: Return D3D11 or D3D12 graphics API's instance.
        result.reset(new GLRenderingInterface(window, config));
#else
        result.reset(new GLRenderingInterface(window, config));
#endif
        break;
    default:
        NXS_ASSERT_MSG(false, "Unknown API type");
        break;
    }
    return result;
}

RenderingInterface::~RenderingInterface()
{
    LOG_INFO(LogRenderingInterface, "~RenderingInterface()");
}

void RenderingInterface::ApplyPipelineState(const PipelineState &state)
{
    if (!glm::all(glm::equal(m_pipelineState.colorMask, state.colorMask)))
    {
        SetColorMask(state.colorMask);
    }

    const DepthFunction depthFunction = state.depthTest ? state.depthFunction : DepthFunction::None;
    if (m_pipelineState.depthFunction != depthFunction)
    {
        SetDepthFunction(depthFunction);
    }

    if (m_pipelineState.depthWrite != state.depthWrite)
    {
        SetDepthWrite(state.depthWrite);
    }

    if (m_pipelineState.overrideBlendMode != state.overrideBlendMode)
    {
        SetBlendMode(state.overrideBlendMode);
    }

    if (m_pipelineState.lineWidth != state.lineWidth)
    {
        SetLineWidth(state.lineWidth);
    }

    if (m_pipelineState.stencilTest != state.stencilTest)
    {
        SetStencilTest(state.stencilTest);
    }

    if (m_pipelineState.stencilOp != state.stencilOp)
    {
        SetStencilOperation(state.stencilOp);
    }

    if (m_pipelineState.polygonMode != state.polygonMode)
    {
        SetPolygonMode(state.polygonMode);
    }

    if (m_pipelineState.cullMode != state.cullMode)
    {
        SetCullMode(state.cullMode);
    }

    if (m_pipelineState.frontFace != state.frontFace)
    {
        SetFrontFace(state.frontFace);
    }

    if (m_pipelineState.globalShader != state.globalShader)
    {
        SetGlobalShader(state.globalShader);
    }

}

void RenderingInterface::SetFrontFace(FrontFace face)
{
    m_pipelineState.frontFace = face;
}

void RenderingInterface::SetDepthFunction(DepthFunction depthFunction)
{
    m_pipelineState.depthFunction = depthFunction;
    m_pipelineState.depthTest = depthFunction != DepthFunction::None;
}
