//
// Created by nutta on 7/25/2025.
//

#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/Container.h"
#include "nexus/graphics/RenderCommand.h"
#include "nexus/scene/renderpass/RenderPass.h"
#include "nexus/geom/Frustum.h"

#include "entt/entity/registry.hpp"

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
    struct LightProperties;
    struct PointLightComponent;
    struct DirectLightComponent;

    class SceneRenderer
    {
    public:
        virtual ~SceneRenderer() = default;

        virtual bool Init(const IGpuResourceFactory& factory, const GraphicsConfig& config);
        bool IsInit() const { return m_initialized; }

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

    protected:
        /**
         * @brief Render the offscreen buffer to screen.
         *
         * @param rs
         */
        void RenderToScreen(RenderSystem& rs);

        HashMap<std::string, Ref<RenderTarget>> m_renderTargets;
        //! Main offscreen buffer
        Ref<RenderTarget> m_screenBuffer;
        Ref<GpuProgram> m_screenShader;
        Ref<VertexBuffer> m_screenVertexBuffer;
        Ref<IndexBuffer> m_screenIndexBuffer;
        bool m_initialized = false;

    private:
        //! A list of render passes sorted by their priority.
        Dict<RenderPassQueue, Ref<RenderPass>> m_passesByQueue;
        Dict<RenderPassQueue, Ref<RenderPass>> m_perFramePasses;
    };
}
