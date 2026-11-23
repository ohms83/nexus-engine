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

#include "SkyboxRenderer.h"

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
    class RenderTarget;
    struct LightProperties;
    struct PointLightComponent;
    struct DirectLightComponent;

    class SceneRenderer
    {
    public:
        virtual ~SceneRenderer() = default;

        virtual bool Init(const IGpuResourceFactory& factory);
        bool IsInit() const { return m_initialized; }

        virtual void Render(RenderSystem& renderSystem, const Scene& scene);

        void RegisterRenderPass(RenderPassQueue queue, Ref<RenderPass> renderPass)
        {
            m_passesByQueue[queue] = renderPass;
        }

        Ref<RenderPass> GetRenderPass(RenderPassQueue queue)
        {
            if(!m_passesByQueue.contains(queue)) return nullptr;
            return m_passesByQueue[queue];
        }

        //! Register an offscreen render target which can be referenced by renderpasses.
        void RegisterRenderTarget(const std::string& name, const Ref<RenderTarget>& target)
        {
            m_renderTargets[name] = target;
        }

        static bool IsSphereInside(const Frustum& viewFustrum, const Sphere& sphere, glm::mat4 modelMtx, const glm::vec3& scale);

    protected:
        static bool RenderInstancedMesh(RenderSystem& renderSystem, const RenderCommand& command, GpuProgram& gpuProgram);
        static void SetAmbientLightParams(GpuProgram& gpuProgram, const entt::registry& registry);
        static void SetDirectLightParams(GpuProgram& gpuProgram, const SceneNode& scene, int32_t shadowLightI = -1);
        static void SetPointLightParams(GpuProgram& gpuProgram, const SceneNode& scene);
        static void SetCameraUniforms(GpuProgram& gpuProgram, const glm::vec3& cameraPos, const glm::mat4& viewMtx, const glm::mat4& projectionMtx);
        void RenderDirectionalShadowMap(RenderSystem& renderSystem, const Scene& scene);
        void SetShadowMapParams(GpuProgram& gpuProgram) const;

        HashMap<std::string, Ref<RenderTarget>> m_renderTargets;
        Ref<RenderTarget> m_shadowMap;
        Ref<GpuProgram> m_shadowProgram;
        Ptr<SkyboxRenderer> m_skyboxRenderer;
        glm::mat4 m_lightSpaceMatrix{1.0f};
        Identifier m_shadowLightId = 0;
        bool m_shadowMapReady = false;
        bool m_initialized = false;

    private:
        //! A list of render passes sorted by their priority.
        Dict<RenderPassQueue, Ref<RenderPass>> m_passesByQueue;
    };
}
