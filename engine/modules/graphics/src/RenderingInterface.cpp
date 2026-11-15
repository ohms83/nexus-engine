//
// Created by nutta on 7/4/2025.
//
#include <filesystem>

#include "nexus/graphics/RenderingInterface.h"
#include "nexus/graphics/RHIPlugin.h"
#include "nexus/graphics/GpuResourceFactory.h"
#include "nexus/debug/Logger.h"
#include "nexus/debug/LogDispatcher.h"

#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_loadso.h>

#include "Remotery.h"

USING_NAMESPACE_NXS;

DEFINE_LOG(RenderingInterface);

namespace
{
    using InitRHIProc = nxs::IRHIPlugin* (*)(const nxs::GraphicsConfig&);
    using GetRHIProc = nxs::IRHIPlugin* (*)();
    using DestroyRHIProc = void (*)();

    struct RHIPluginModule
    {
        SDL_SharedObject* module = nullptr; // Retained so plugin-created objects remain callable.
        nxs::IRHIPlugin* plugin = nullptr;
        InitRHIProc init = nullptr;
        GetRHIProc get = nullptr;
        DestroyRHIProc destroy = nullptr;
        bool initialized = false;

        ~RHIPluginModule()
        {
            if (initialized && destroy)
            {
                destroy();
            }
        }
    };

    RHIPluginModule& GetRHIPluginModule()
    {
        static RHIPluginModule runtime;
        return runtime;
    }

    nxs::IRHIPlugin* LoadOpenGLPlugin(const nxs::GraphicsConfig& config)
    {
        auto& runtime = GetRHIPluginModule();
        if (runtime.plugin)
        {
            return runtime.plugin;
        }

        if (!runtime.module)
        {
            const char* basePath = SDL_GetBasePath();
            if (!basePath)
            {
                LOG_FATAL(LogRenderingInterface,
                    std::format("Failed to locate application directory: {}", SDL_GetError()));
                return nullptr;
            }

#if defined(_WIN32)
            constexpr auto libraryName = "nexus-rhi-opengl.dll";
#elif defined(__APPLE__)
            constexpr auto libraryName = "nexus-rhi-opengl.dylib";
#else
            constexpr auto libraryName = "nexus-rhi-opengl.so";
#endif
            const auto path = std::filesystem::path(basePath) / "plugins" / libraryName;
            const auto utf8Path = path.u8string();
            runtime.module = SDL_LoadObject(reinterpret_cast<const char*>(utf8Path.c_str()));
            if (!runtime.module)
            {
                LOG_FATAL(LogRenderingInterface,
                    std::format("Failed to load RHI plugin '{}': {}", path.string(), SDL_GetError()));
                return nullptr;
            }

            runtime.init = reinterpret_cast<InitRHIProc>(SDL_LoadFunction(runtime.module, "InitRHI"));
            runtime.get = reinterpret_cast<GetRHIProc>(SDL_LoadFunction(runtime.module, "GetRHI"));
            runtime.destroy = reinterpret_cast<DestroyRHIProc>(SDL_LoadFunction(runtime.module, "DestroyRHI"));
            if (!runtime.init || !runtime.get || !runtime.destroy)
            {
                LOG_FATAL(LogRenderingInterface,
                    std::format("RHI plugin '{}' is missing one or more required entry points", path.string()));
                SDL_UnloadObject(runtime.module);
                runtime.module = nullptr;
                runtime.init = nullptr;
                runtime.get = nullptr;
                runtime.destroy = nullptr;
                return nullptr;
            }
        }

        if (!runtime.init(config))
        {
            LOG_FATAL(LogRenderingInterface, "Failed to initialize the OpenGL RHI plugin");
            runtime.destroy();
            return nullptr;
        }

        runtime.plugin = runtime.get();
        if (!runtime.plugin)
        {
            LOG_FATAL(LogRenderingInterface, "OpenGL RHI plugin returned an empty plugin instance");
            runtime.destroy();
            return nullptr;
        }

        runtime.initialized = true;
        return runtime.plugin;
    }
}

NXS_NAMESPACE
{
    IGpuResourceFactory* GetGpuResourceFactory()
    {
        auto* plugin = GetRHIPluginModule().plugin;
        NXS_ASSERT_MSG(plugin, "RHI plugin is not initialized");
        return plugin->GetGpuResourceFactory();
    }
}

Ref<RenderingInterface> RenderingInterface::Create(WindowContext window, const GraphicsConfig& config)
{
    LOG_INFO(LogRenderingInterface, std::format("Creating Rendering Interface API: {}", GraphicsAPIToString(config.api)));
    if (config.api != GraphicsAPI::OpenGL && config.api != GraphicsAPI::Undefined)
    {
        NXS_ASSERT_MSG(false, "Unknown API type");
        return nullptr;
    }

    auto* plugin = LoadOpenGLPlugin(config);
    if (!plugin)
    {
        return nullptr;
    }

    auto renderingInterface = plugin->CreateRenderingInterface(window, config);
    if (!renderingInterface)
    {
        LOG_FATAL(LogRenderingInterface, "OpenGL RHI plugin returned an empty rendering interface");
    }
    return renderingInterface;
}

RenderingInterface::~RenderingInterface()
{
    LOG_INFO(LogRenderingInterface, "~RenderingInterface()");
}

void RenderingInterface::ApplyPipelineState(const PipelineState &state)
{
    rmt_ScopedCPUSample(RenderingInterface_ApplyPipelineState, 0);
    if (!glm::all(glm::equal(m_pipelineState.colorMask, state.colorMask)))
    {
        SetColorMask(state.colorMask);
    }

    const DepthFunction depthFunction = state.depthTest ? state.depthFunction : DepthFunction::None;
    if (m_pipelineState.depthFunction != depthFunction)
    {
        SetDepthFunction(depthFunction);
    }

    if (m_pipelineState.depthWrite != state.depthWrite)
    {
        SetDepthWrite(state.depthWrite);
    }

    if (m_pipelineState.overrideBlendMode != state.overrideBlendMode)
    {
        SetBlendMode(state.overrideBlendMode);
    }

    if (m_pipelineState.lineWidth != state.lineWidth)
    {
        SetLineWidth(state.lineWidth);
    }

    if (m_pipelineState.stencilTest != state.stencilTest)
    {
        SetStencilTest(state.stencilTest);
    }

    if (m_pipelineState.stencilOp != state.stencilOp)
    {
        SetStencilOperation(state.stencilOp);
    }

    if (m_pipelineState.polygonMode != state.polygonMode)
    {
        SetPolygonMode(state.polygonMode);
    }

    if (m_pipelineState.cullMode != state.cullMode)
    {
        SetCullMode(state.cullMode);
    }

    if (m_pipelineState.frontFace != state.frontFace)
    {
        SetFrontFace(state.frontFace);
    }

    if (m_pipelineState.globalShader != state.globalShader)
    {
        SetGlobalShader(state.globalShader);
    }

}

void RenderingInterface::SetFrontFace(FrontFace face)
{
    m_pipelineState.frontFace = face;
}

void RenderingInterface::SetDepthFunction(DepthFunction depthFunction)
{
    m_pipelineState.depthFunction = depthFunction;
    m_pipelineState.depthTest = depthFunction != DepthFunction::None;
}
