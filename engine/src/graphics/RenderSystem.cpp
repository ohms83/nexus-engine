#include <format>
#include <iostream>
#include <nexus/graphics/RenderSystem.h>
#include <nexus/graphics/Shader.h>

USING_NAMESPACE_NXS;

RenderSystem::RenderSystem(const WindowContext window, const GraphicsConfig& config)
    : m_config(config)
{
    m_renderingInterface = RenderingInterface::Create(window, config);
}

RenderSystem::~RenderSystem()
{
    RenderingInterface::Destroy();
    m_renderingInterface = nullptr;
}

void RenderSystem::ClearScreen() const
{
    m_renderingInterface->ClearBuffer(m_clearColor, m_clearDepth);
}

void RenderSystem::BeginDraw()
{
    m_timer.Stamp();
    ClearScreen();
    m_renderingInterface->SetViewport(0, 0, m_config.screenWidth, m_config.screenHeight);
}

void RenderSystem::Draw()
{
    m_drawCount = 0;
    m_polygonCount = 0;

    for (const auto& command : m_renderCommands)
    {
        m_renderingInterface->Draw(command);
        m_drawCount++;
        m_polygonCount += command.GetPolygonCount();
    }
    m_frameIndex++;

    m_renderCommands.clear();
}

void RenderSystem::EndDraw()
{
    m_renderingInterface->SwapBuffer();
    m_renderTime = m_timer.GetDeltaTime();
}

void RenderSystem::OnResize(const uint32 pixel_w, const uint32 pixel_h)
{
    m_config.screenWidth = pixel_w;
    m_config.screenHeight = pixel_h;
    m_renderingInterface->OnResize(pixel_w, pixel_h);
}

void RenderSystem::RegisterDrawCommand(const RenderCommand& command)
{
    m_renderCommands.emplace_back(command);
}
