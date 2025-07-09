//
// Created by nutta on 7/7/2025.
//

#pragma once

#include <nexus/NxsDefine.h>
#include <string>

NXS_NAMESPACE
{
    // TODO:
    // enum class RenderDebugFlags : uint32
    // {
    //     None        = 0,
    //     //! Enable wireframe for all primitives.
    //     WireFrame   = UINT32_C(0x00000001),
    //     /// Enable infinitely fast hardware test. No draw calls will be submitted to the graphics driver.
    //     /// It's useful when profiling to quickly assess bottleneck between CPU and GPU.
    //     IFS         = UINT32_C(0x00000002),
    //     //! Enable statistics display.
    //     Stats       = UINT32_C(0x00000004),
    //     //! Enable debug text display.
    //     Text        = UINT32_C(0x00000008),
    //     //! Enable profiler. This causes per-view statistics to be collected, available through `bgfx::Stats::ViewStats`. This is unrelated to the profiler functions in `bgfx::CallbackI`.
    //     Profiler    = UINT32_C(0x00000010),
    //     //! Enable everything except the IFS.
    //     NoIFS       = UINT32_C(0x000000fd),
    //     //! All debug features will be available
    //     All         = UINT32_C(0x000000ff),
    // };

    enum class GraphicsAPI
    {
        /// If set to @c Undefined, the engine will automatically decide
        /// the most suitable API to use.
        Undefined,
        OpenGL,
        OpenGLES,
        Vulkan,
        D3D11,
        D3D12,
        Num
    };

    inline const std::string& GraphicsAPIToString(GraphicsAPI api)
    {
        static const std::array<std::string, SIZE_CAST(GraphicsAPI::Num)> apiNames = {
            "Undefined",
            "OpenGL",
            "OpenGLES",
            "Vulkan",
            "D3D11",
            "D3D12",
        };
        assert(api < GraphicsAPI::Num);
        return apiNames[INT_CAST(api)];
    }

    enum class DrawMode
    {
        Point,
        Line,
        LineLoop,
        LineStrip,
        Triangle,
        TriangleStrip,
        TriangleFan,
        Quad,
        Num
    };
}
