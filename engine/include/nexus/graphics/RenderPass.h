#pragma once

#include "GraphicsConst.h"

#define RENDER_PASS_DEPTH_FILL  0
#define RENDER_PASS_OPAQUE      1000
#define RENDER_PASS_ALPHA       2000
#define RENDER_PASS_OVERLAY     10000

NXS_NAMESPACE
{
    struct RenderPass
    {
        //! Render-pass name. Mainly used for debugging.
        std::string name;
        uint32_t priority;
        DepthFunction depthFunction = DepthFunction::None;
        StencilOperation stencilOp = StencilOperation::Zero;
    };
}