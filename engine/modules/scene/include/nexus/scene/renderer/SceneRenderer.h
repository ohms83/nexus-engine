//
// Created by nutta on 7/25/2025.
//

#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/Container.h"
#include "nexus/graphics/RenderCommand.h"
#include "nexus/scene/renderpass/RenderPass.h"
#include "nexus/geom/Frustum.h"
#include "nexus/memory/CopyBuffer.h"

#include "entt/entity/registry.hpp"
#include "sigslot/signal.hpp"

#include <algorithm>
#include <string>
#include <unordered_map>

DECLARE_LOG_EXTERN(SceneRenderer);

NXS_NAMESPACE
{
    class RenderSystem;
    class Scene;
    class Shader;
    class Mesh;
    class GpuProgram;
    class VertexBuffer;
    class IndexBuffer;
    class RenderTarget;
    class TaskScheduler;
    class Timer;
    struct LightProperties;
    struct PointLightComponent;
    struct DirectLightComponent;

    class SceneRenderer
    {
    public:
        SceneRenderer();
        virtual ~SceneRenderer();

        virtual bool Init(const IGpuResourceFactory* factory, RenderSystem& rs);
        bool IsInit() const { return m_initialized; }

        void SetTaskScheduler(Ref<TaskScheduler> scheduler);

        virtual void Render(RenderSystem& rs, const Scene& scene);

        void RegisterRenderPass(RenderPassQueue queue, Ref<RenderPass> renderPass)
        {
            m_passesByQueue[queue] = renderPass;
        }

        Ref<RenderPass> GetRenderPass(RenderPassQueue queue)
        {
            if(!m_passesByQueue.contains(queue)) return nullptr;
            return m_passesByQueue[queue];
        }

        void RegisterPerFrameRenderPass(RenderPassQueue queue, Ref<RenderPass> renderPass)
        {
            m_perFramePasses[queue] = renderPass;
        }

        Ref<RenderPass> GetPerFrameRenderPass(RenderPassQueue queue)
        {
            if(!m_perFramePasses.contains(queue)) return nullptr;
            return m_perFramePasses[queue];
        }

        //! Register an offscreen render target which can be referenced by renderpasses.
        void RegisterRenderTarget(const std::string& name, const Ref<RenderTarget>& target)
        {
            m_renderTargets[name] = target;
        }

        void SetAutoExposureEnabled(bool enabled) { m_autoExposureEnabled = enabled; }
        NODISCARD bool IsAutoExposureEnabled() const { return m_autoExposureEnabled; }
        void SetAutoExposureTarget(float targetLuminance) { m_autoExposureTarget = std::max(0.001f, targetLuminance); }
        NODISCARD float GetAutoExposureTarget() const { return m_autoExposureTarget; }
        void SetAutoExposureSpeed(float speed) { m_autoExposureSpeed = std::max(0.0f, speed); }
        NODISCARD float GetAutoExposureSpeed() const { return m_autoExposureSpeed; }
        NODISCARD float GetExposure() const { return m_exposure; }

    protected:
        /**
         * @brief Render the offscreen buffer to screen.
         *
         * @param rs
         */
        void RenderToScreen(RenderSystem& rs);

        void OnViewportResize(uint32_t w, uint32_t h);
        sigslot::scoped_connection m_viewportResizeConnection;

        HashMap<std::string, Ref<RenderTarget>> m_renderTargets;
        //! Main offscreen buffer
        Ref<RenderTarget> m_screenBuffer;
        Ref<GpuProgram> m_screenShader;
        Ref<VertexBuffer> m_screenVertexBuffer;
        Ref<IndexBuffer> m_screenIndexBuffer;
        const IGpuResourceFactory* m_gpuResourceFactory = nullptr;
        bool m_initialized = false;
        bool m_autoExposureEnabled = true;
        float m_autoExposureTarget = 0.18f;
        float m_autoExposureSpeed = 1.0f;
        float m_exposure = 1.0f;

    private:
        bool InitResources(const IGpuResourceFactory& factory, const GraphicsConfig& config);
        void CreateOffscreenBuffer(int32_t width, int32_t height);

        bool UpdateAutoExposure();

        //! A list of render passes sorted by their priority.
        Dict<RenderPassQueue, Ref<RenderPass>> m_passesByQueue;
        Dict<RenderPassQueue, Ref<RenderPass>> m_perFramePasses;

        Ref<TaskScheduler> m_taskScheduler;
        TaskID m_luminanceSamplingTask = NXS_INVALID_ID;
        TaskID m_updateExposureTask = NXS_INVALID_ID;
        Ref<CopyBuffer> m_screenTexels;
        Ref<Timer> m_timer;
    };
}
