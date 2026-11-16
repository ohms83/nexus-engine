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
    class RenderTarget;
    struct LightProperties;
    struct PointLightComponent;
    struct DirectLightComponent;

    class SceneRenderer
    {
    public:
        virtual ~SceneRenderer() = default;
        bool Init();
        virtual void Render(RenderSystem& renderSystem, const Scene& scene);

        void RegisterRenderPass(Ref<RenderPass> renderPass)
        {
            m_renderPasses.push_back(renderPass);
        }

        void SortRenderPasses()
        {
            std::ranges::sort(m_renderPasses, [this](const Ref<const RenderPass>& a, const Ref<const RenderPass>& b) {
                return a->priority < b->priority;
            });
        }

        //! Register an offscreen render target which can be referenced by renderpasses.
        void RegisterRenderTarget(const std::string& name, const Ref<RenderTarget>& target)
        {
            m_renderTargets[name] = target;
        }

        static bool IsSphereInside(const Frustum& viewFustrum, const Sphere& sphere, glm::mat4 modelMtx, const glm::vec3& scale);

    protected:
        static bool RenderInstancedMesh(RenderSystem& renderSystem, const RenderCommand& command, Ref<GpuProgram> gpuProgram);
        static void SetAmbientLightParams(Ref<GpuProgram> gpuProgram, const entt::registry& registry);
        void SetDirectLightParams(Ref<GpuProgram> gpuProgram, const SceneNode& scene) const;
        static void SetPointLightParams(Ref<GpuProgram> gpuProgram, const SceneNode& scene);
        void RenderDirectionalShadowMap(RenderSystem& renderSystem, const Scene& scene);
        void SetShadowMapParams(Ref<GpuProgram> gpuProgram) const;

        static void SetCameraUniforms(Ref<GpuProgram> gpuProgram, const glm::vec3& cameraPos, const glm::mat4& viewMtx, const glm::mat4& projectionMtx);

        //! A list of render passes sorted by their priority.
        Array<Ref<RenderPass>> m_renderPasses;
        HashMap<std::string, Ref<RenderTarget>> m_renderTargets;
        Ref<RenderTarget> m_shadowMap;
        Ref<GpuProgram> m_shadowProgram;
        glm::mat4 m_lightSpaceMatrix{1.0f};
        Identifier m_shadowLightId = 0;
        bool m_shadowMapReady = false;
        bool m_initialized = false;
    };
}
