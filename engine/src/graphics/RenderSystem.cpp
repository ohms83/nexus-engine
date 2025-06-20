#include <nexus/graphics/RenderSystem.h>
#include <bgfx/bgfx.h>
#include <map>

USING_NAMESPACE_NXS;

static bgfx::RendererType::Enum NexusAPIToBgfxRenderType(GraphicsAPI api)
{
    std::map<GraphicsAPI, bgfx::RendererType::Enum> renderTypeMap = {
        {GraphicsAPI::OpenGL, bgfx::RendererType::Enum::OpenGL},
        {GraphicsAPI::OpenGLES, bgfx::RendererType::Enum::OpenGLES},
        {GraphicsAPI::D3D11, bgfx::RendererType::Enum::Direct3D11},
        {GraphicsAPI::D3D12, bgfx::RendererType::Enum::Direct3D12},
        {GraphicsAPI::Vulkan, bgfx::RendererType::Enum::Vulkan},
        {GraphicsAPI::Undefined, bgfx::RendererType::Enum::Count},
    };
    return renderTypeMap[api];
}

RenderSystem::RenderSystem(RenderSystemConfig config)
    : m_config(config)
{
    bgfx::Init init;
    init.type = NexusAPIToBgfxRenderType(config.api);
    init.vendorId = BGFX_PCI_ID_NONE; // Auto-select adapter.
    init.profile = CAST<int>(config.debugFlags) & CAST<int>(RenderDebugFlags::Profiler) > 0 ? true : false;
    init.platformData.nwh = config.windowHandle;
    init.resolution.height = config.screenHeight;
    init.resolution.width = config.screenWidth;
    init.resolution.reset = config.vsync ? BGFX_RESET_VSYNC : BGFX_RESET_NONE;
#ifdef _DEBUG
    init.debug = true;
#else
    init.debug = false;
#endif
    bgfx::init(init);

#ifdef _DEBUG
    bgfx::setDebug(true);
#endif
}

RenderSystem::~RenderSystem()
{
    bgfx::shutdown();
}

void RenderSystem::ClearScreen() const
{
    // Set view 0 default viewport.
    bgfx::setViewRect(0,
        0, 0,
        CAST<uint16_t>(m_config.screenWidth), CAST<uint16_t>(m_config.screenHeight));

    // This dummy draw call is here to make sure that view 0 is cleared
    // if no other draw calls are submitted to view 0.
    bgfx::touch(0);
}

void RenderSystem::BeginDraw() const
{
    bgfx::setViewClear(
        0,
        BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
        m_clearColor.RGBA(),
        1,
        0);
}

void RenderSystem::Draw()
{

}

void RenderSystem::EndDraw()
{
    bgfx::frame();
}