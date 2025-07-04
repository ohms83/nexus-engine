#include <nexus/graphics/RenderSystem.h>
#include <nexus/graphics/Shader.h>

USING_NAMESPACE_NXS;

RenderSystem::RenderSystem(GraphicsConfig config)
    : m_config(config)
{
}

RenderSystem::~RenderSystem()
{
    RenderingInterface::Destroy();
}

void RenderSystem::ClearScreen() const
{
}

void RenderSystem::BeginDraw() const
{
}

void RenderSystem::Draw()
{

}

void RenderSystem::EndDraw()
{
}

void RenderSystem::OnResize(const uint32_t width, const uint32_t height)
{
}
