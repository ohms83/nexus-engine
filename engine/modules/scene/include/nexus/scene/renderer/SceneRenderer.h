//
// Created by nutta on 7/25/2025.
//

#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/graphics/RenderCommand.h"
#include "nexus/graphics/RenderPass.h"
#include "nexus/geom/Frustum.h"

#include "entt/entity/registry.hpp"

#include <algorithm>
#include <string>
#include <unordered_map>

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

    DECLARE_LOG_EXTERN(SceneRenderer);

    class SceneRenderer
    {
    public:
        virtual ~SceneRenderer() = default;
        virtual void Render(RenderSystem& renderSystem, const Scene& scene) = 0;

        void RegisterRenderPass(const RenderPass& renderPass)
        {
            m_renderPasses.push_back(renderPass);
        }

        void SortRenderPasses()
        {
            std::ranges::sort(m_renderPasses, std::ranges::less{}, &RenderPass::priority);
        }

        NODISCARD const std::vector<RenderPass>& GetRenderPasses() const { return m_renderPasses; }

        //! Register an offscreen render target which can be referenced by renderpasses.
        void RegisterRenderTarget(const std::string& name, const Ref<RenderTarget>& target)
        {
            m_renderTargets[name] = target;
        }

        static bool IsSphereInside(const Frustum& viewFustrum, const Sphere& sphere, glm::mat4 modelMtx, const glm::vec3& scale);

    protected:
        static RenderCommand CreateRenderCommand(Ref<const Mesh> mesh, glm::mat4&& modelMtx, const glm::mat4& mvpMtx);
        static bool RenderInstancedMesh(RenderSystem& renderSystem, const RenderCommand& command, Ref<GpuProgram> gpuProgram);
        static void SetAmbientLightParams(Ref<GpuProgram> gpuProgram, const entt::registry& registry);
        static void SetDirectLightParams(Ref<GpuProgram> gpuProgram, const entt::registry& registry);
        static void SetPointLightParams(Ref<GpuProgram> gpuProgram, const entt::registry& registry);

        static void SetCameraUniforms(Ref<GpuProgram> gpuProgram, const glm::vec3& cameraPos, const glm::mat4& viewMtx, const glm::mat4& projectionMtx);

        //! A list of render passes sorted by their priority.
        std::vector<RenderPass> m_renderPasses;
        std::unordered_map<std::string, Ref<RenderTarget>> m_renderTargets;
    };
}
