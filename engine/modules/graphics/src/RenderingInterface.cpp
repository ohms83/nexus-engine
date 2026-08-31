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
