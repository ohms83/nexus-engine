#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/graphics/GraphicsConfig.h"

NXS_NAMESPACE
{
    class NXS_API IGpuResourceFactory;

    class IRHIPlugin
    {
    public:
        virtual ~IRHIPlugin() = default;

        virtual IGpuResourceFactory* GetGpuResourceFactory() = 0;
    };
    
    extern "C" {
        MAYBE_UNUSED NXS_API IRHIPlugin* InitRHI(const GraphicsConfig& config);
        NODISCARD NXS_API IRHIPlugin* GetRHI();
        NXS_API void DestroyRHI();
    }
}