#include "RenderSystem.hpp"

#include "GraphicsConst.h"
#include "opengl/GLRenderSystem.hpp"

#include <exception>
#include <sstream>

USING_NAMESPACE_NXS;

RenderSystem::RenderSystem()
{

}

RenderSystem::~RenderSystem()
{

}

const Color4F& RenderSystem::getClearColor() const
{
    return _clearColor;
}
void RenderSystem::setClearColor(const Color4F& color)
{
    _clearColor = color;
}

RenderSystem* RenderSystem::create(int renderSystem)
{
    RenderSystem* result = nullptr;
    std::stringstream ss;

    switch (renderSystem)
    {
    case RENDER_SYSTEM_GL:
        result = new GLRenderSystem();
        break;
    case RENDER_SYSTEM_VULKAN:
        ss << "RENDER_SYSTEM_VULKAN is not implemented!";
        break;
    default:
        ss << "Unknown render system! ID=" << renderSystem;
        break;
    }
    
    if (!ss.str().empty()) {
        throw std::runtime_error(ss.str());
    }

    return result;
}
