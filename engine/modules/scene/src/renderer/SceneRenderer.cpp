//
// Created by nutta on 7/25/2025.
//
#include "nexus/scene/renderer/SceneRenderer.h"
#include "nexus/Container.h"

#include <format>
#include <algorithm>
#include <cmath>
#include <limits>

#include "nexus/debug/Logger.h"
#include "nexus/graphics/debug/Gizmos.h"
#include "nexus/graphics/RenderSystem.h"
#include "nexus/graphics/RenderCommand.h"
#include "nexus/graphics/RenderTarget.h"
#include "nexus/graphics/GpuResourceFactory.h"
#include "nexus/graphics/Shader.h"
#include "nexus/graphics/RenderCommandBatcher.h"
#include "nexus/graphics/RenderCommand.h"
#include "nexus/geom/Frustum.h"
#include "nexus/ecs/Ecs.h"
#include "nexus/memory/OwningBuffer.h"
#include "nexus/scene/RenderGraph.h"
#include "nexus/scene/Scene.h"
#include "nexus/scene/component/LightComponent.h"
#include "nexus/scene/component/MeshComponent.h"
#include "nexus/math/Matrix.h"

#include "Remotery.h"
#include "nexus/graphics/GraphicsProfiler.h"

USING_NAMESPACE_NXS;

DEFINE_LOG(SceneRenderer);

void SceneRenderer::RenderDirectionalShadowMap(RenderSystem& renderSystem, SceneRenderingContext& sceneCtx)
{
    rmt_ScopedCPUSample(SceneRenderer_RenderDirectionalShadowMap, 0);
    m_shadowMapReady = false;
    m_shadowLightId = 0;
    const auto& registry = *sceneCtx.scene->GetRegistry();

    SceneNode* lightNode = nullptr;
    const DirectLightComponent* shadowLight = nullptr;
    for (const auto view = registry.view<const SceneNodeComponent, const DirectLightComponent>();
         const auto& [entity, nodeComponent, light] : view.each())
    {
        lightNode = SceneNode::FindNode(nodeComponent.id);
        if (!lightNode)
        {
            LOG_ERROR(LogSceneRenderer, std::format(
                "Directional shadow map light node {} was not found.", nodeComponent.id));
            continue;
        }
        if (!lightNode->IsActive()) continue;
        if (!light.properties.canCastShadows) continue;

        shadowLight = &light;
        break;
    }

    if (!shadowLight || !lightNode)
    {
        return;
    }

    const glm::vec3 unnormalizedDirection =
        lightNode->GetGlobalOrientation() * shadowLight->direction;
    const float directionLength = glm::length(unnormalizedDirection);
    if (!std::isfinite(directionLength) || directionLength < 0.000001f)
    {
        LOG_WARNING(LogSceneRenderer, "Directional shadow map is disabled because the light direction is invalid.");
        return;
    }
    const glm::vec3 lightDirection = unnormalizedDirection / directionLength;

    struct ShadowMesh
    {
        Ref<Mesh> mesh;
        glm::mat4 modelMatrix;
    };
    Array<ShadowMesh> shadowMeshes;
    {
        rmt_ScopedCPUSample(SceneRenderer_CalculateLightBounds, 0);
        glm::vec3 boundsMin(std::numeric_limits<float>::max());
        glm::vec3 boundsMax(std::numeric_limits<float>::lowest());
        bool hasBounds = false;
        for (const auto view = registry.view<const SceneNodeComponent, const MeshComponent>();
            const auto& [entity, nodeComponent, meshComponent] : view.each())
        {
            auto* node = SceneNode::FindNode(nodeComponent.id);
            const auto mesh = meshComponent.GetMesh();
            if (!node || !mesh || !node->IsActive()) continue;

            const auto material = mesh->GetMaterial();
            if (!material || (!material->castShadow && !material->receiveShadows)) continue;

            const glm::mat4 modelMatrix = Matrix::ExpandAffine(node->GetGlobalAffineTransform());
            const glm::vec3 center = glm::vec3(modelMatrix * glm::vec4(mesh->GetSphere().center, 1.0f));
            const float maxScale = std::max({
                glm::length(glm::vec3(modelMatrix[0])),
                glm::length(glm::vec3(modelMatrix[1])),
                glm::length(glm::vec3(modelMatrix[2]))});
            const float radius = std::max(mesh->GetSphere().radius * maxScale, 0.0f);
            boundsMin = glm::min(boundsMin, center - radius);
            boundsMax = glm::max(boundsMax, center + radius);
            if (material->castShadow) shadowMeshes.push_back({mesh, modelMatrix});
            hasBounds = true;
        }

        if (!hasBounds || shadowMeshes.empty())
        {
            return;
        }

        const glm::vec3 center = (boundsMin + boundsMax) * 0.5f;
        const float radius = std::max(glm::length(boundsMax - boundsMin) * 0.5f, 1.0f);
        const float lightDistance = radius * 2.0f + 10.0f;
        const glm::vec3 up = std::abs(glm::dot(lightDirection, glm::vec3(0.0f, 1.0f, 0.0f))) > 0.98f
            ? glm::vec3(0.0f, 0.0f, 1.0f)
            : glm::vec3(0.0f, 1.0f, 0.0f);
        const glm::mat4 lightView = glm::lookAt(
            center - lightDirection * lightDistance,
            center,
            up);
        const glm::mat4 lightProjection = glm::ortho(
            -radius,
            radius,
            -radius,
            radius,
            0.1f,
            lightDistance + radius * 2.0f);
        m_lightSpaceMatrix = lightProjection * lightView;
    }

    constexpr uint32_t shadowMapResolution = 2048;
    if (!m_shadowMap)
    {
        m_shadowMap = GetGpuResourceFactory()->CreateRenderTarget(
            shadowMapResolution,
            shadowMapResolution,
            true);
    }
    if (!m_shadowMap)
    {
        LOG_ERROR(LogSceneRenderer, "Failed to create a directional shadow map render target.");
        return;
    }

    if (!m_shadowProgram)
    {
        auto shader = std::make_shared<Shader>("_DirectionalShadowMap", 0);
        constexpr auto vertexSource = R"(
            #version 330 core
            layout (location = 0) in vec3 aPos;
            uniform mat4 _Model;
            uniform mat4 _LightSpaceMatrix;
            void main()
            {
                gl_Position = _LightSpaceMatrix * _Model * vec4(aPos, 1.0);
            }
        )";
        constexpr auto fragmentSource = R"(
            #version 330 core
            void main() {}
        )";
        if (!shader->CompileFromSource(vertexSource, fragmentSource, ""))
        {
            LOG_ERROR(LogSceneRenderer, "Failed to compile the directional shadow map shader.");
            return;
        }
        m_shadowProgram = shader->GetGpuProgram();
    }

    m_shadowMap->Bind(renderSystem);
    auto renderInterface = renderSystem.GetRenderInterface();
    PipelineState shadowState;
    shadowState.depthTest = true;
    shadowState.depthWrite = true;
    shadowState.depthFunction = DepthFunction::Lesser;
    shadowState.overrideBlendMode = BlendMode::None;
    renderInterface->ApplyPipelineState(shadowState);
    renderInterface->ClearDepth(1.0f);
    m_shadowProgram->Bind();
    m_shadowProgram->SetUniformMatrix("_LightSpaceMatrix", m_lightSpaceMatrix, false);

    {
        rmt_ScopedCPUSample(SceneRenderer_RenderShadowMap, 0);
        rmt_ScopedOpenGLSample(SceneRenderer_RenderShadowMap);
        for (const auto& shadowMesh : shadowMeshes)
        {
            rmt_ScopedCPUSample(SceneRenderer_RenderShadowMesh, 0)
            const auto& mesh = shadowMesh.mesh;
            if (!mesh->GetVertexBuffer() || !mesh->GetIndexBuffer()) continue;

            const auto indexBuffer = mesh->GetIndexBuffer();
            m_shadowProgram->SetUniformMatrix(
                "_Model",
                shadowMesh.modelMatrix,
                false);
            mesh->GetVertexBuffer()->Bind();
            renderSystem.DrawIndexed(indexBuffer);
        }
    }

    m_shadowProgram->Unbind();
    m_shadowMap->Unbind(renderSystem);
    m_shadowLightId = lightNode->GetId();
    m_shadowMapReady = true;

    sceneCtx.shadowMaps.emplace_back(ShadowMap {
        .lightSpaceMatrix = m_lightSpaceMatrix,
        .depthMap = m_shadowMap->GetDepthTextureProxy()
    });
}

bool SceneRenderer::Init(const IGpuResourceFactory& factory)
{
    if (m_initialized) return true;

    for (auto& [queue, pass] : m_passesByQueue)
    {
        if (!pass->Init(factory)) return false;
    }
    return m_initialized = true;
}

void SceneRenderer::Render(RenderSystem &renderSystem, const Scene &scene)
{
    rmt_ScopedCPUSample(SceneRenderer_Render, 0);

    SceneRenderingContext sceneCtx = {
        .scene = &scene
    };

    RenderDirectionalShadowMap(renderSystem, sceneCtx);

    Array<RenderCommand> commands;
    commands.reserve(1024);

    struct QueuedRenderPass
    {
        int priority;
        RenderPass* pass;
    };
    struct ViewPasses
    {
        const ISceneView* view;
        ViewContext viewCtx;
        Array<QueuedRenderPass> passes;
    };
    Array<ViewPasses> views;
    for (const auto view : ISceneView::GetViewRegistry())
    {
        const auto* node = view->GetViewNode();
        if (node && (!node->IsActive() || node->GetRegistry() != scene.GetRegistry())) continue;

        auto viewCtx = view->CreateViewContext(scene);
        ViewPasses viewPasses {
            view,
            viewCtx,
        };

        const auto& renderProfile = viewCtx.profile;
        const auto addPass = [&](RenderPassQueue queue, bool selected) {
            if (!selected) return;
            const auto itr = m_passesByQueue.find(queue);
            if (itr != m_passesByQueue.end() && itr->second->enabled) {
                viewPasses.passes.emplace_back(QueuedRenderPass {
                    INT_CAST(itr->first),
                    itr->second.get()
                });
            }
        };
        addPass(RenderPassQueue::DepthFill, renderProfile.depthFill);
        addPass(RenderPassQueue::Opaque, !renderProfile.IsCull(CullingFlags::Opaque));
        addPass(RenderPassQueue::Environment, !renderProfile.IsCull(CullingFlags::Environment));
        addPass(RenderPassQueue::Alpha, !renderProfile.IsCull(CullingFlags::Alpha));
        addPass(RenderPassQueue::Overlay, !renderProfile.IsCull(CullingFlags::Overlay));
        std::ranges::sort(viewPasses.passes, {}, &QueuedRenderPass::priority);
        views.push_back(viewPasses);
    }

    for (const auto& viewPasses : views)
    {
        const auto* view = viewPasses.view;
        const auto& viewCtx = viewPasses.viewCtx;
        if (viewCtx.target) {
            viewCtx.target->Bind(renderSystem);
        }

        viewCtx.ClearView(*renderSystem.GetRenderInterface());

        for (const auto& queuedPass : viewPasses.passes)
        {
            auto& pass = queuedPass.pass;
            pass->Begin(renderSystem, sceneCtx);
            pass->Render(renderSystem, viewCtx);
            pass->End(renderSystem, sceneCtx);
        }

        if (viewCtx.target) {
            viewCtx.target->Unbind(renderSystem);
        }
    }
}
