#include <format>
#include <iostream>
#include "nexus/graphics/RenderSystem.h"
#include "nexus/graphics/Shader.h"

#include "nexus/core/LogDispatcher.h"
#include "nexus/time/HighResTimeSource.h"
#include "time/StandardTimeSource.h"

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
    for (auto& commandBuffer : m_renderCommands) commandBuffer.clear();

    RenderingInterface::Destroy();
    m_renderingInterface = nullptr;
}

void RenderSystem::ClearScreen() const
{
    m_renderingInterface->ClearBuffer(m_clearColor, m_clearDepth);
}

void RenderSystem::BeginDraw()
{
    m_timer.Tick();
    ClearScreen();
}

void RenderSystem::Draw()
{
    m_drawCount = 0;
    m_polygonCount = 0;

    for (auto& commandList : m_renderCommands)
    {
        for (const auto& command : commandList)
        {
            m_renderingInterface->SetDepthFunction(command.depthFunction);
            m_renderingInterface->Draw(command);
            m_drawCount++;
            m_polygonCount += command.GetPolygonCount();
        }
        commandList.clear();
    }
    m_frameIndex++;
}

void RenderSystem::EndDraw()
{
    m_renderingInterface->SwapBuffer();
    m_renderTime = m_timer.GetDeltaTime() * 1000.f;
}

void RenderSystem::OnResize(const uint32 pixel_w, const uint32 pixel_h)
{
    m_config.screenWidth = CAST<int>(pixel_w);
    m_config.screenHeight = CAST<int>(pixel_h);
    m_renderingInterface->OnResize(pixel_w, pixel_h);
}

void RenderSystem::RegisterDrawCommand(const RenderCommand& command, RenderPass pass)
{
    auto& commands = m_renderCommands[INT_CAST(pass)];
    commands.emplace_back(command);
}
