#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/ApplicationContext.h"
#include "nexus/graphics/GraphicsConfig.h"

#if defined(_WIN32)
    #if defined(NXS_RHI_PLUGIN_EXPORTS)
        #define NXS_RHI_PLUGIN_API __declspec(dllexport)
    #else
        #define NXS_RHI_PLUGIN_API __declspec(dllimport)
    #endif
#else
    #define NXS_RHI_PLUGIN_API __attribute__((visibility("default")))
#endif

NXS_NAMESPACE
{
    class IGpuResourceFactory;
    class RenderingInterface;

    class IRHIPlugin
    {
    public:
        virtual ~IRHIPlugin() = default;

        virtual IGpuResourceFactory* GetGpuResourceFactory() = 0;
        virtual Ref<RenderingInterface> CreateRenderingInterface(
            WindowContext window,
            const GraphicsConfig& config) = 0;
    };
    
    extern "C" {
        MAYBE_UNUSED NXS_RHI_PLUGIN_API IRHIPlugin* InitRHI(const GraphicsConfig& config);
        NODISCARD NXS_RHI_PLUGIN_API IRHIPlugin* GetRHI();
        NXS_RHI_PLUGIN_API void DestroyRHI();
    }
}