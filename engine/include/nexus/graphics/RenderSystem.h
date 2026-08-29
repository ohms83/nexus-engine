#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/ApplicationContext.h"
#include "nexus/time/Timer.h"
#include "nexus/debug/LogDispatcher.h"
#include "nexus/core/Color.h"
#include "RenderingInterface.h"
#include "PipelineState.h"

NXS_NAMESPACE
{
    class RenderSystem final
    {
    public:
        explicit RenderSystem(WindowContext window, const GraphicsConfig& config);
        explicit RenderSystem(Ref<RenderingInterface> renderingInterface);

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
        void DrawIndexed(Ref<IndexBuffer> indexBuffer);
        void DrawIndexedInstanced(Ref<IndexBuffer> indexBuffer, uint32_t instanceCount);
        void EndDraw();

        //! An event handler called when the window resize event occured.
        void OnResize(uint32_t pixel_w, uint32_t pixel_h);

        NODISCARD Ref<RenderingInterface> GetRenderInterface() const
        {
            NXS_ASSERT_MSG(m_renderingInterface, "Rendering interface is not initialized");
            return m_renderingInterface;
        }

        NODISCARD RenderContext GetRenderContext() const
        {
            NXS_ASSERT_MSG(m_renderingInterface, "Rendering interface is not initialized");
            return m_renderingInterface->GetRenderContext();
        }

        //! Get the time used for rendering the last frame in milliseconds.
        NODISCARD float GetrenderTime() const
        {
            return m_renderTime;
        }

        NODISCARD uint32_t GetFrameIndex() const
        {
            return m_frameIndex;
        }

        //! Get the number of draw-calls from the previous frame.
        NODISCARD uint32_t GetDrawCount() const
        {
            return m_drawCount;
        }

        //! Get the total number of polygons drawn in the previous frame.
        NODISCARD uint32_t GetPolygonCount() const
        {
            return m_polygonCount;
        }

        NODISCARD const GraphicsConfig& GetGraphicsConfig() const
        {
            return m_config;
        }

        //! Apply pipeline state with minimal redundant state transitions.
        void ApplyPipelineState(const PipelineState& state);
        void SetGlobalShader(Ref<GpuProgram> shader);

    protected:
        // State cache to reduce redundant calls to the rendering backend
        DepthFunction m_cachedDepthFunction = DepthFunction::None;
        bool m_cachedDepthMask = true;
        PolygonMode m_cachedPolygonMode = PolygonMode::Fill;
        PolygonFacing m_cachedCullMode = PolygonFacing::Back;
        FrontFace m_cachedFrontFace = FrontFace::CounterClockWise;
        Ref<GpuProgram> m_cachedGlobalShader;
        GraphicsConfig m_config;
        Ref<RenderingInterface> m_renderingInterface;
        Color4F m_clearColor = Color4F::Grey;
        float m_clearDepth = 1.0f;

        uint32_t m_frameIndex = 0;
        uint32_t m_drawCount = 0;
        uint32_t m_polygonCount = 0;

        //! Time spent rendering the last frame in milliseconds. Mainly used for profiling.
        float m_renderTime = 0.0f;
        Timer m_timer;
    };
}