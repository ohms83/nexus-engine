#pragma once

#include <nexus/NxsDefine.h>
#include "Color.h"

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
    };

    enum class RenderDebugFlags : uint32_t
    {
        None        = 0,
        //! Enable wireframe for all primitives.
        WireFrame   = UINT32_C(0x00000001),
        /// Enable infinitely fast hardware test. No draw calls will be submitted to the graphics driver.
        /// It's useful when profiling to quickly assess bottleneck between CPU and GPU.
        IFS         = UINT32_C(0x00000002),
        //! Enable statistics display.
        Stats       = UINT32_C(0x00000004),
        //! Enable debug text display.
        Text        = UINT32_C(0x00000008),
        //! Enable profiler. This causes per-view statistics to be collected, available through `bgfx::Stats::ViewStats`. This is unrelated to the profiler functions in `bgfx::CallbackI`.
        Profiler    = UINT32_C(0x00000010),
        //! Enable everything except the IFS.
        NoIFS       = UINT32_C(0x000000fd),
        //! All debug features will be available
        All         = UINT32_C(0x000000ff),
    };

    struct RenderSystemConfig
    {
        GraphicsAPI api;
        int32_t screenWidth;
        int32_t screenHeight;
        //! Enable/disable VSync.
        bool vsync;
        RenderDebugFlags debugFlags;
        //! Native window handle. MUST be provided by the application.
        void* windowHandle;
    };

    class RenderSystem
    {
    public:
        explicit RenderSystem(RenderSystemConfig config);
        virtual ~RenderSystem();

        void ClearScreen() const;

        void SetClearColor(const Color4B& color = COLOR4B_GREY)
        {
            m_clearColor = color;
        }

        void SetClearColor(const uint32_t rgba)
        {
            m_clearColor = rgba;
        }

        [[nodiscard]] const Color4B& GetClearColor() const
        {
            return m_clearColor;
        }

        void SetClearDepth(const float depth)
        {
            m_clearDepth = depth;
        }

        [[nodiscard]] float GetClearDepth() const
        {
            return m_clearDepth;
        }
        
        void BeginDraw() const;
        void Draw();
        void EndDraw();

    protected:
        RenderSystemConfig m_config;
        Color4B m_clearColor = COLOR4B_GREY;
        float m_clearDepth = 1.0f;
    };
}