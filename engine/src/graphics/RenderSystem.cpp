#include <format>
#include <iostream>
#include "nexus/graphics/RenderSystem.h"
#include "nexus/graphics/Shader.h"

#include "nexus/core/LogDispatcher.h"
#include "nexus/time/HighResTimeSource.h"
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
