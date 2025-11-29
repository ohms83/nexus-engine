#pragma once

#include "GraphicsConst.h"
#include "GpuProgram.h"

NXS_NAMESPACE
{
    /**
     * @brief Represents the state of the graphics pipeline.
     * 
     */
    struct PipelineState
    {
        //! Whether depth testing is enabled.
        bool depthTest = false;
        //! Whether depth writing is enabled.
        bool depthWrite = false;
        //! The depth comparison function to use.
        DepthFunction depthFunction = DepthFunction::Lesser;
        //! Whether stencil testing is enabled.
        bool stencilTest = false;
        //! The stencil operation to use.
        StencilOperation stencilOp = StencilOperation::Zero;
        //! The blend mode to override with.
        BlendMode overrideBlendMode = BlendMode::None;
        //! The stencil operation to use.
        PolygonMode polygonMode = PolygonMode::Fill;
        //! The face culling mode to use.
        PolygonFacing cullMode = PolygonFacing::Back;
        //! The front face winding order.
        FrontFace frontFace = FrontFace::CounterClockWise;
        //! Global shader program to use for this pipeline stage.
        Ref<GpuProgram> globalShader = nullptr;
    };
}