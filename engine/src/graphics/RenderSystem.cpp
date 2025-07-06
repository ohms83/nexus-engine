#include <nexus/graphics/RenderSystem.h>
#include <nexus/graphics/Shader.h>

USING_NAMESPACE_NXS;

RenderSystem::RenderSystem(WindowContext window, GraphicsConfig config)
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

void RenderSystem::BeginDraw() const
{
}

void RenderSystem::Draw()
{
}

void RenderSystem::EndDraw() const
{
    m_renderingInterface->SwapBuffer();
}

void RenderSystem::OnResize(const uint32 pixel_w, const uint32 pixel_h) const
{
    m_renderingInterface->OnResize(pixel_w, pixel_h);
}
