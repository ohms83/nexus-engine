#include <format>
#include <iostream>
#include "nexus/graphics/RenderSystem.h"
#include "nexus/graphics/Shader.h"
#include "nexus/debug/LogDispatcher.h"
#include "nexus/time/HighResTimeSource.h"
#include "nexus/time/StandardTimeSource.h"
#include "nexus/graphics/GraphicsProfiler.h"
#include "nexus/graphics/RenderDocCapture.h"

#include "Remotery.h"

USING_NAMESPACE_NXS;

DEFINE_LOG(RenderSystem);

RenderSystem::RenderSystem(Ref<RenderingInterface> rhi, const IGpuResourceFactory* factory)
    : m_config(rhi->GetConfig())
    , m_rhi(rhi)
    , m_gpuFactory(factory)
    , m_timer(std::make_shared<HighResTimeSource>())
{
    m_timer.Start();
}

RenderSystem::~RenderSystem()
{
    m_rhi.reset();
}

void RenderSystem::ClearScreen() const
{
    rmt_ScopedCPUSample(RenderSystem_ClearScreen, 0);
    m_rhi->ClearBuffer(
        m_rhi->GetClearColor(),
        m_rhi->GetClearDepth()
    );
}

void RenderSystem::BeginDraw()
{
    rmt_ScopedCPUSample(RenderSystem_BeginDraw, 0);
    // Mark the start of a frame for Remotery with a dynamic name
    rmt_BeginCPUSampleDynamic("Frame", 0);
    RenderDocCapture::BeginCaptureIfRequested();
    GraphicsProfiler::BeginFrame();
    m_timer.Tick();
    m_rhi->SetFramebufferSRGB(m_rhi->IsFramebufferSRGBSupported());
    ClearScreen();

    m_drawCount = 0;
    m_polygonCount = 0;
}

void RenderSystem::DrawIndexed(Ref<IndexBuffer> indexBuffer)
{
    m_rhi->DrawIndexed(indexBuffer);
    m_drawCount++;
    m_polygonCount += indexBuffer->NumPolygons();
}

void RenderSystem::DrawIndexedInstanced(Ref<IndexBuffer> indexBuffer, uint32_t instanceCount)
{
    m_rhi->DrawIndexedInstanced(indexBuffer, instanceCount);
    m_drawCount++;
    m_polygonCount += indexBuffer->NumPolygons() * instanceCount;
}

void RenderSystem::EndDraw()
{
    rmt_ScopedCPUSample(EndDraw, 0);
    {
        rmt_ScopedCPUSample(SwapBuffer, 0);
        m_rhi->SwapBuffer();
    }
    // End RenderDoc capture if it was active for this frame
    RenderDocCapture::EndCaptureIfActive();
    // Let Remotery gather GPU timestamps for the frame
    rmt_MarkFrame();
    // Report and end frame sampling
    GraphicsProfiler::EndFrameAndReport();

    // End dynamic frame CPU sample
    rmt_EndCPUSample();
    m_renderTime = m_timer.GetDeltaTime() * 1000.f;
    m_frameIndex++;
}

void RenderSystem::OnResize(const uint32_t pixel_w, const uint32_t pixel_h)
{
    m_config.screenWidth = CAST<int>(pixel_w);
    m_config.screenHeight = CAST<int>(pixel_h);
    m_rhi->OnResize(pixel_w, pixel_h);
    onViewportResize(pixel_w, pixel_h);
}
