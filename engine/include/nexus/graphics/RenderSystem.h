#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/time/Timer.h"
#include "nexus/core/LogDispatcher.h"
#include "Color.h"
#include "RenderingInterface.h"

NXS_NAMESPACE
{
    enum class RenderPass
    {
        //! Depth fill pass
        DepthFill,
        Opaque,
        AlphaBlend,
        //! Gizmos will always be rendered last.
        Gizmo,
        Num,
    };

    class RenderSystem final
    {
    public:
        explicit RenderSystem(WindowContext window, const GraphicsConfig& config);
        ~RenderSystem();

        void ClearScreen() const;

        void SetClearColor(const Color4F& color)
        {
            m_clearColor = color;
        }

        void SetClearColor(const uint32_t rgba)
        {
            m_clearColor = rgba;
        }

        NODISCARD const Color4F& GetClearColor() const
        {
            return m_clearColor;
        }

        void SetClearDepth(const float depth)
        {
            m_clearDepth = depth;
        }

        NODISCARD float GetClearDepth() const
        {
            return m_clearDepth;
        }
        
        void BeginDraw();
        void Draw();
        void EndDraw();

        //! An event handler called when the window resize event occured.
        void OnResize(uint32_t pixel_w, uint32_t pixel_h);

        void RegisterDrawCommand(const RenderCommand& command, RenderPass pass = RenderPass::Opaque);

        NODISCARD RenderingInterface& GetRenderInterface() const
        {
            NXS_ASSERT_MSG(m_renderingInterface, "Rendering interface is not initialized");
            assert(m_renderingInterface);
            // ReSharper disable once CppDFANullDereference
            return *m_renderingInterface;
        }

        NODISCARD RenderContext GetRenderContext() const
        {
            NXS_ASSERT_MSG(m_renderingInterface, "Rendering interface is not initialized");
            // ReSharper disable once CppDFANullDereference
            return m_renderingInterface->GetRenderContext();
        }

        //! Get the time used for rendering the last frame in milliseconds.
        NODISCARD float GetrenderTime() const
        {
            return m_renderTime;
        }

        NODISCARD uint32 GetFrameIndex() const
        {
            return m_frameIndex;
        }

        //! Get the number of draw-calls from the previous frame.
        NODISCARD uint32 GetDrawCount() const
        {
            return m_drawCount;
        }

        //! Get the total number of polygons drawn in the previous frame.
        NODISCARD uint32 GetPolygonCount() const
        {
            return m_polygonCount;
        }

        NODISCARD const GraphicsConfig& GetGraphicsConfig() const
        {
            return m_config;
        }

    protected:
        using CommandBuffer = std::vector<RenderCommand>;

        GraphicsConfig m_config;
        RenderingInterface* m_renderingInterface = nullptr;
        Color4F m_clearColor = COLOR4F_GREY;
        float m_clearDepth = 1.0f;
        std::array<CommandBuffer, INT_CAST(RenderPass::Num)> m_renderCommands;

        uint32 m_frameIndex = 0;
        uint32 m_drawCount = 0;
        uint32 m_polygonCount = 0;

        //! Time spent rendering the last frame in milliseconds. Mainly used for profiling.
        float m_renderTime = 0.0f;
        Timer m_timer;
    };
}