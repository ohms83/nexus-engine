#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/ApplicationContext.h"
#include "nexus/time/Timer.h"
#include "nexus/debug/LogDispatcher.h"
#include "nexus/core/Color.h"
#include "nexus/Container.h"
#include "nexus/memory/Memory.h"
#include "nexus/geom/Rect.h"

#include "RenderTarget.h"
#include "RenderingInterface.h"
#include "GpuResourceFactory.h"

#include "sigslot/signal.hpp"

NXS_NAMESPACE
{
    class RenderSystem final
    {
    public:
        explicit RenderSystem(Ref<RenderingInterface> rhi, const IGpuResourceFactory* factory);

        ~RenderSystem();

        void ClearScreen() const;

        void BeginDraw();
        void DrawIndexed(Ref<IndexBuffer> indexBuffer);
        void DrawIndexedInstanced(Ref<IndexBuffer> indexBuffer, uint32_t instanceCount);
        void EndDraw();

        void ReadFrameData(FrameBufferType type, Ref<IBuffer> outData) const;
        void ReadFrameSubData(FrameBufferType type, const iRect& region, Ref<IBuffer> outData) const;

        //! An event handler called when the window resize event occured.
        void OnResize(uint32_t pixel_w, uint32_t pixel_h);

        NODISCARD Ref<RenderingInterface> GetRenderInterface() const
        {
            NXS_ASSERT_MSG(m_rhi, "Rendering interface is not initialized");
            return m_rhi;
        }

        NODISCARD RenderContext GetRenderContext() const
        {
            NXS_ASSERT_MSG(m_rhi, "Rendering interface is not initialized");
            return m_rhi->GetRenderContext();
        }

        //! Get the time used for rendering the last frame in milliseconds.
        NODISCARD float GetrenderTime() const
        {
            return m_renderTime;
        }

        NODISCARD float GetDeltaTime() const
        {
            return m_timer.GetDeltaTime();
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

        sigslot::signal<uint32_t, uint32_t> onViewportResize;

    protected:
        //! Graphics configuration used to initialize the rendering interface.
        GraphicsConfig m_config;
        Ref<RenderingInterface> m_rhi;
        const IGpuResourceFactory* m_gpuFactory;

        uint32_t m_frameIndex = 0;
        uint32_t m_drawCount = 0;
        uint32_t m_polygonCount = 0;

        //! Time spent rendering the last frame in milliseconds. Mainly used for profiling.
        float m_renderTime = 0.0f;
        Timer m_timer;
    };
}