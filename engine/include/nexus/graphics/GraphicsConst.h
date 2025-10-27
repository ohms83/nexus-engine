//
// Created by nutta on 7/7/2025.
//

#pragma once

#include <array>
#include <nexus/NxsDefine.h>
#include <string>

NXS_NAMESPACE
{
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

    enum class PolygonFacing
    {
        None,
        Front,
        Back,
        Both,
    };

    enum class FrontFace
    {
        ClockWise,
        CounterClockWise,
    };

    enum class ProjectionType
    {
        Perspective,
        Orthographic,
        Num
    };

    enum class DepthFunction
    {
        //! Disable depth testing.
        None,
        Lesser,
        LesserOrEqual,
        Equal,
        Greater,
        Always,
    };

    enum class StencilOperation
    {
        Keep,
        Zero,
        Replace,
        Increment,
        IncrementWrap,
        Decrement,
        DecrementWrap,
        Invert,
    };
    /**
     *  @brief Defines alpha-blend flags.
     */
    enum class BlendMode
    {
        /**
         * No blending.
         */
        None,
        /**
         * Alpha blending. This is the most commonly used mode.
         *
         * Formula:
         * @code
         * SourceColor*SourceAlpha + DestColor*(1-SourceAlpha)
         * @endcode
         */
        Alpha,
        /**
         * Additive blending
         *
         * Formula:
         * @code
         * SourceColor*1 + DestColor*1
         * @endcode
         */
        Additive,
    };

    enum class PolygonMode
    {
        Fill,
        Wireframe,
        Point,
    };

    enum class DrawBuffer
    {
        None,
        Front,
        Back,
        Num,
    };
}
