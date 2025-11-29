#include <format>
#include <iostream>
#include "graphics/RenderSystem.h"
#include "graphics/Shader.h"
#include "core/LogDispatcher.h"
#include "time/HighResTimeSource.h"
#include "time/StandardTimeSource.h"

#include "Remotery.h"

USING_NAMESPACE_NXS;

DEFINE_LOG(RenderSystem);

RenderSystem::RenderSystem(const WindowContext window, const GraphicsConfig& config)
    : m_config(config)
    , m_timer(std::make_shared<HighResTimeSource>())
{
    m_renderingInterface = RenderingInterface::Create(window, config);
    m_timer.Start();
}

RenderSystem::~RenderSystem()
{
    m_renderingInterface.reset();
}

void RenderSystem::ClearScreen() const
{
    rmt_ScopedCPUSample(RenderSystem_ClearScreen, 0);
    m_renderingInterface->ClearBuffer(m_clearColor, m_clearDepth);
}

void RenderSystem::BeginDraw()
{
    rmt_ScopedCPUSample(RenderSystem_BeginDraw, 0);
    m_timer.Tick();
    ClearScreen();

    m_drawCount = 0;
    m_polygonCount = 0;
}

void RenderSystem::DrawIndexed(Ref<IndexBuffer> indexBuffer)
{
    m_renderingInterface->DrawIndexed(indexBuffer);
    m_drawCount++;
    m_polygonCount += indexBuffer->NumPolygons();
}

void RenderSystem::DrawIndexedInstanced(Ref<IndexBuffer> indexBuffer, uint32 instanceCount)
{
    m_renderingInterface->DrawIndexedInstanced(indexBuffer, instanceCount);
    m_drawCount++;
    m_polygonCount += indexBuffer->NumPolygons() * instanceCount;
}

void RenderSystem::EndDraw()
{
    rmt_ScopedCPUSample(EndDraw, 0);
    {
        rmt_ScopedCPUSample(SwapBuffer, 0);
        m_renderingInterface->SwapBuffer();
    }
    m_renderTime = m_timer.GetDeltaTime() * 1000.f;
    m_frameIndex++;
}

void RenderSystem::OnResize(const uint32 pixel_w, const uint32 pixel_h)
{
    m_config.screenWidth = CAST<int>(pixel_w);
    m_config.screenHeight = CAST<int>(pixel_h);
    m_renderingInterface->OnResize(pixel_w, pixel_h);
}

void RenderSystem::ApplyPipelineState(const PipelineState& state)
{
    if (state.depthTest)
    {
        if (m_cachedDepthFunction != state.depthFunction)
        {
            m_renderingInterface->SetDepthFunction(state.depthFunction);
            m_cachedDepthFunction = state.depthFunction;
        }
    }
    else if (m_cachedDepthFunction != DepthFunction::None)
    {
        m_renderingInterface->SetDepthFunction(DepthFunction::None);
        m_cachedDepthFunction = DepthFunction::None;
    }

    if (m_cachedDepthMask != state.depthWrite)
    {
        m_renderingInterface->SetDepthMask(state.depthWrite);
        m_cachedDepthMask = state.depthWrite;
    }

    if (m_cachedPolygonMode != state.polygonMode)
    {
        m_renderingInterface->SetPolygonMode(state.polygonMode);
        m_cachedPolygonMode = state.polygonMode;
    }

    if (m_cachedCullMode != state.cullMode)
    {
        m_renderingInterface->SetCullMode(state.cullMode);
        m_cachedCullMode = state.cullMode;
    }

    if (m_cachedFrontFace != state.frontFace)
    {
        m_renderingInterface->SetFrontFace(state.frontFace);
        m_cachedFrontFace = state.frontFace;
    }

    if (m_cachedGlobalShader != state.globalShader)
    {
        m_renderingInterface->SetGlobalShader(state.globalShader);
        m_cachedGlobalShader = state.globalShader;
    }
}

void RenderSystem::SetGlobalShader(Ref<GpuProgram> shader)
{
    if (m_cachedGlobalShader != shader)
    {
        m_renderingInterface->SetGlobalShader(shader);
        m_cachedGlobalShader = shader;
    }
}
