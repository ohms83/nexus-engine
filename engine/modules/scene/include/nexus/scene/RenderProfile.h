#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/core/Color.h"
#include "nexus/graphics/GraphicsConst.h"
#include "nexus/scene/renderpass/RenderPass.h"

NXS_NAMESPACE
{
    enum class CullingFlags
    {
        None    = 0x00,
        Opaque  = 0x01,
        Alph    = 0x02,
        Overlay = 0x04,
        All     = 0xFFFFFFFF,
    };

    struct RenderProfile
    {
        //!
        ClearFlags clearFlags = ClearFlags::All;
        Color3F clearColor { 0x303030FF };
        float clearDepth = 1.0f;
        uint32_t clearStencil = 0;
        bool depthFill = false;
        CullingFlags culling = CullingFlags::None;
    };
}