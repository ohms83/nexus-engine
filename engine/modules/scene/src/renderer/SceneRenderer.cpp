//
// Created by nutta on 7/25/2025.
//
#include "nexus/scene/renderer/SceneRenderer.h"
#include "nexus/Container.h"

#include <format>
#include <algorithm>
#include <cmath>
#include <cstring>
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

inline void CreateRenderCommand(
    RenderCommand& cmd,
    const Mesh* mesh,
    const glm::mat4x3 &modelMtx,
    const glm::mat4 &viewProjMtx,
    CommandSortType sortType)
{
    rmt_ScopedCPUSample(CreateRenderCommand, 0);
    const auto material = mesh->GetMaterial();
    const auto meshSphere = mesh->GetSphere();
    // Transform sphere's center to world using only a model matrix
    const glm::vec3 worldCenter = glm::vec3(modelMtx * glm::vec4(meshSphere.center, 1.0f));
    // Transform only the world's z and w coordinates to the clipping plane;
    // eliminates unnecessary x and y transform.
    const float z = viewProjMtx[0][2] * worldCenter.x + viewProjMtx[1][2] * worldCenter.y + viewProjMtx[2][2] * worldCenter.z + viewProjMtx[3][2];
    const float w = viewProjMtx[0][3] * worldCenter.x + viewProjMtx[1][3] * worldCenter.y + viewProjMtx[2][3] * worldCenter.z + viewProjMtx[3][3];
    const float clipZ = z / w;
    const bool translucent = (material->blendMode != BlendMode::None);
    const uint32_t materialId = 0x7FFFFFFF & material->GetId();
    const float depthN = clipZ; // normalized in -1..1
    const float depthNormalized = (depthN + 1.0f) * 0.5f;
    cmd.vertexBuffer = mesh->GetVertexBuffer();
    cmd.indexBuffer = mesh->GetIndexBuffer();
    cmd.indexCount = mesh->GetIndexBuffer()->GetNumIndexDraw();
    cmd.indexOffset = 0;
    cmd.vertexOffset = 0;
    cmd.modelMatrix = modelMtx;
    cmd.layerMask = 0xFFFFFFFFu;
    cmd.material = material;
    cmd.SetSortKey(
        translucent,
        materialId,
        sortType == CommandSortType::FrontToBack ? depthNormalized : 1.0f - depthNormalized);
}

inline bool IsNodeActive(const SceneNodeComponent& nodeComp)
{
    const auto* node = SceneNode::FindNode(nodeComp.id);
    if (!node)
    {
        LOG_ERROR(LogSceneRenderer, std::format("Scene node {} was not found.", nodeComp.id));
        return false;
    }
    return node->IsActive();
}

bool SceneRenderer::RenderInstancedMesh(RenderSystem& renderSystem, const RenderCommand& command, GpuProgram& gpuProgram)
{
    rmt_ScopedCPUSample(SceneRenderer_RenderInstancedMesh, 0);
    if (command.instanceModels.empty()) return false;

    // Build an OwningBuffer holding all model matrices
    const auto instanceCount = static_cast<uint32_t>(command.instanceModels.size());
    const size_t bytes = instanceCount * sizeof(glm::mat4);
    uint8_t* mem = new uint8_t[bytes];
    // Copy matrices
    for (uint32_t i = 0; i < instanceCount; ++i)
    {
        std::memcpy(&mem[i * sizeof(glm::mat4)], &command.instanceModels[i], sizeof(glm::mat4));
    }
    Ref<IBuffer> instanceBuffer = std::make_shared<OwningBuffer>(mem, bytes);

    // Create instance attributes for mat4 as 4 vec4 attributes (layout locations 10..13)
    Array<VertexAttribute> attrs;
    for (int i = 0; i < 4; ++i)
    {
        VertexAttribute a;
        a.type = VertexAttribute::Type::TexCoord0; // type is unused if attribIndex set
        a.dataType = DataType::Float;
        a.numElements = 4;
        a.attribIndex = 10 + i; // locations 10..13
        a.divisor = 1;
        attrs.push_back(a);
    }

    command.vertexBuffer->AttachInstanceStream(std::static_pointer_cast<IBuffer>(instanceBuffer), attrs, BufferUsage::StaticDraw);
    // Record composition stats for instanced draw
    GraphicsProfiler::RecordProgramDraw(gpuProgram.GetHandle(), command.indexCount, instanceCount);
    GraphicsProfiler::RecordMaterialDraw(command.material ? command.material->GetId() : 0, command.indexCount, instanceCount);
    renderSystem.DrawIndexedInstanced(command.indexBuffer, instanceCount);
    command.vertexBuffer->DetachInstanceStreams();
    // free temporary allocation (OwningBuffer::Take took ownership)
    // OwningBuffer destructor will free it
    return true;
}

void SceneRenderer::SetAmbientLightParams(GpuProgram& gpuProgram, const entt::registry &registry)
{
    rmt_ScopedCPUSample(SceneRenderer_SetAmbientLightParams, 0);
    const auto ambientLightEnt = registry.view<AmbientLightComponent>().front();
    const auto color = ambientLightEnt == entt::null ? Color3F::Grey : registry.get<AmbientLightComponent>(ambientLightEnt).color;
    gpuProgram.SetUniformVector("_AmbientLight", color);
}

void SceneRenderer::SetDirectLightParams(GpuProgram& gpuProgram, const SceneNode& scene, int32_t shadowLightId)
{
    rmt_ScopedCPUSample(SceneRenderer_SetDirectLightParams, 0);
    int32_t numLight = 0;
    int32_t shadowLightIndex = -1;
    const auto registry = scene.GetRegistry();
    for (const auto view = registry->view<SceneNodeComponent, DirectLightComponent>(); const auto& [entity, node, light] : view.each())
    {
        if (!IsNodeActive(node)) continue;

        rmt_BeginCPUSample(CreateUniformNames, 0);
        // TODO: Cache the uniform names.
        const auto uniformLocation = std::format("_DirectLights[{}]", numLight);
        const auto uniformLocationColor = std::format("{}.properties.color", uniformLocation);
        const auto uniformLocationDiffuse = std::format("{}.properties.diffuseIntensity", uniformLocation);
        const auto uniformLocationSpecular = std::format("{}.properties.specularIntensity", uniformLocation);
        const auto uniformLocationCutoff = std::format("{}.properties.cutoff", uniformLocation);
        const auto uniformLocationDirection = std::format("{}.direction", uniformLocation);
        rmt_EndCPUSample();

        rmt_BeginCPUSample(EmplaceUniformValues, 0);
        SceneNode* nodePtr = SceneNode::FindNode(node.id);
        if (!nodePtr)
        {
            LOG_ERROR(LogSceneRenderer, std::format("SceneRenderer::SetDirectLightParams: SceneNode with ID {} not found.", node.id));
            rmt_EndCPUSample();
            continue;
        }
        const auto direction = nodePtr->GetGlobalOrientation() * light.direction;
        shadowLightIndex = node.id == shadowLightId ? numLight : -1;
        gpuProgram.SetUniformVector(uniformLocationColor, light.properties.color);
        gpuProgram.SetUniformVector(uniformLocationDirection, glm::normalize(direction));
        gpuProgram.SetUniformFloat(uniformLocationDiffuse, light.properties.diffuseIntensity);
        gpuProgram.SetUniformFloat(uniformLocationSpecular, light.properties.specularIntensity);
        gpuProgram.SetUniformFloat(uniformLocationCutoff, light.properties.cutoffRange);
        rmt_EndCPUSample();

        numLight++;
    }
    gpuProgram.SetUniformInt("_NumDirectLight", numLight);
    gpuProgram.SetUniformInt("_ShadowLightIndex", shadowLightIndex);
}

void SceneRenderer::SetPointLightParams(GpuProgram& gpuProgram, const SceneNode& scene)
{
    rmt_ScopedCPUSample(SceneRenderer_SetPointLightParams, 0);
    int32_t numLight = 0;
    const auto registry = scene.GetRegistry();
    for (const auto view = registry->view<SceneNodeComponent, PointLightComponent>(); const auto& [entity, node, light] : view.each())
    {
        if (!IsNodeActive(node)) continue;

        rmt_BeginCPUSample(CreateUniformNames, 0);
        // TODO: Cache the uniform names.
        const auto uniformLocation = std::format("_PointLights[{}]", numLight);
        const auto uniformLocationColor = std::format("{}.properties.color", uniformLocation);
        const auto uniformLocationDiffuse = std::format("{}.properties.diffuseIntensity", uniformLocation);
        const auto uniformLocationSpecular = std::format("{}.properties.specularIntensity", uniformLocation);
        const auto uniformLocationCutOff = std::format("{}.cutoff", uniformLocation);
        const auto uniformLocationPosition = std::format("{}.position", uniformLocation);
        const auto uniformLocationConst = std::format("{}.constant", uniformLocation);
        const auto uniformLocationLinear = std::format("{}.linear", uniformLocation);
        const auto uniformLocationQuad = std::format("{}.quadratic", uniformLocation);
        rmt_EndCPUSample();

        rmt_BeginCPUSample(EmplaceUniformValues, 0);
        SceneNode* nodePtr = SceneNode::FindNode(node.id);
        if (!nodePtr) {
            LOG_ERROR(LogSceneRenderer, std::format("SceneRenderer::SetPointLightParams: SceneNode with ID {} not found.", node.id));
            rmt_EndCPUSample();
            continue;
        }
        gpuProgram.SetUniformVector(uniformLocationPosition, nodePtr->GetGlobalPosition());
        gpuProgram.SetUniformVector(uniformLocationColor, light.properties.color);
        gpuProgram.SetUniformFloat(uniformLocationDiffuse, light.properties.diffuseIntensity);
        gpuProgram.SetUniformFloat(uniformLocationSpecular, light.properties.specularIntensity);
        gpuProgram.SetUniformFloat(uniformLocationCutOff, light.properties.cutoffRange);
        gpuProgram.SetUniformFloat(uniformLocationConst, light.constant);
        gpuProgram.SetUniformFloat(uniformLocationLinear, light.linear);
        gpuProgram.SetUniformFloat(uniformLocationQuad, light.quadratic);
        rmt_EndCPUSample();

        numLight++;
    }
    gpuProgram.SetUniformInt("_NumPointLight", numLight);
}

void SceneRenderer::SetCameraUniforms(GpuProgram& gpuProgram, const glm::vec3& cameraPos, const glm::mat4& viewMtx, const glm::mat4& projectionMtx)
{
    rmt_ScopedCPUSample(SceneRenderer_SetCameraUniforms, 0);
    gpuProgram.SetUniformVector("_CameraPos", cameraPos);
    gpuProgram.SetUniformMatrix("_View", viewMtx, false);
    gpuProgram.SetUniformMatrix("_Projection", projectionMtx, false);
}

void SceneRenderer::RenderDirectionalShadowMap(RenderSystem& renderSystem, const Scene& scene)
{
    rmt_ScopedCPUSample(SceneRenderer_RenderDirectionalShadowMap, 0);
    m_shadowMapReady = false;
    m_shadowLightId = 0;
    const auto& registry = *scene.GetRegistry();

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
}

void SceneRenderer::SetShadowMapParams(GpuProgram& gpuProgram) const
{
    rmt_ScopedCPUSample(SceneRenderer_SetShadowMapParams, 0);
    gpuProgram.SetUniformInt("_HasShadowMap", m_shadowMapReady ? 1 : 0);
    if (!m_shadowMapReady)
    {
        return;
    }

    gpuProgram.SetUniformMatrix("_LightSpaceMatrix", m_lightSpaceMatrix, false);
    gpuProgram.SetUniformTexture2D(
        "_ShadowMap",
        m_shadowMap->GetDepthTextureProxy(),
        15);
}


bool SceneRenderer::Init()
{
    if (m_initialized) return true;
    for (auto& [queue, pass] : m_passesByQueue)
    {
        if (!pass->Init()) return false;
    }

    m_skyboxRenderer = NewPtr<SkyboxRenderer>(*GetGpuResourceFactory());
    return m_initialized = true;
}

void SceneRenderer::Render(RenderSystem &renderSystem, const Scene &scene)
{
    rmt_ScopedCPUSample(SceneRenderer_Render, 0);

    RenderDirectionalShadowMap(renderSystem, scene);

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

        // TODO: Append the offscreen targets from the ShadowPass
        Array<AttachmentDesc> offscreenTargets;
        for (const auto& queuedPass : viewPasses.passes)
        {
            const auto pass = queuedPass.pass;
            rmt_BeginCPUSampleDynamic(pass->name.c_str(), 0);
            rmt_BeginOpenGLSampleDynamic(pass->name.c_str());

            pass->Begin(renderSystem, offscreenTargets);
            commands.clear();
            const auto& visibleNodes = viewCtx.visibleNodes;
            const auto globalShader = pass->pipelineState.globalShader;

            rmt_BeginCPUSample(SceneRenderer_CreateRenderCommands, 0);
            for (const auto node : viewCtx.visibleNodes)
            {
                const auto meshComp = node->GetComponent<MeshComponent>();
                if (!meshComp) continue;

                auto mesh = meshComp->GetMesh();
                if (!mesh || !pass->IsPassFiltered(mesh)) continue;

                const auto& modelMtx = node->GetGlobalAffineTransform();
                CreateRenderCommand(commands.emplace_back(RenderCommand {}),
                    mesh.get(),
                    modelMtx,
                    viewCtx.viewProjectionMatrix,
                    pass->commandSortType);
            }

            pass->AppendCommands(renderSystem, visibleNodes, commands);
            rmt_EndCPUSample();

            {
                rmt_ScopedCPUSample(SceneRenderer_SortMeshes, 0)
                std::ranges::sort(commands, [](const RenderCommand& a, const RenderCommand& b) {
                    return a.sortKey < b.sortKey;
                });
            }

            {
                rmt_ScopedCPUSample(SceneRenderer_RenderMeshes, 0)

                Ref<GpuProgram> usingProgram = nullptr;
                Ref<Material> usingMaterial = nullptr;
                for (const auto& cmd : commands)
                {
                    if (!cmd.material) continue;
                    auto material = cmd.material;

                    Ref<GpuProgram> gpuProgram = nullptr;
                    if (globalShader) {
                        gpuProgram = globalShader;
                    }
                    else {
                        if (usingMaterial != material)
                        {
                            gpuProgram = material->Use();
                            GraphicsProfiler::RecordMaterialSwitching(material->GetId());
                            usingMaterial = material;
                        }
                        else
                        {
                            gpuProgram = material->GetGpuProgram();
                        }
                    }
                    NXS_ASSERT_MSG(gpuProgram, "No shader program available for rendering");

                    if (gpuProgram != usingProgram)
                    {
                        if (!gpuProgram->IsBinding()) gpuProgram->Bind();

                        if (gpuProgram->FindUniform("_OutputLinearColor") >= 0)
                        {
                            gpuProgram->SetUniformInt(
                                "_OutputLinearColor",
                                (viewCtx.target ||
                                 renderSystem.GetRenderInterface()->IsFramebufferSRGBSupported()) ? 1 : 0);
                        }

                        SetCameraUniforms(*gpuProgram, viewCtx.position, viewCtx.viewMatrix, viewCtx.projectionMatrix);
                        SetAmbientLightParams(*gpuProgram, *scene.GetRegistry());
                        SetDirectLightParams(*gpuProgram, scene, m_shadowMapReady ? m_shadowLightId : NXS_INVALID_ID);
                        SetPointLightParams(*gpuProgram, scene);
                        SetShadowMapParams(*gpuProgram);
                        usingProgram = gpuProgram;
                    }

                    if (!RenderInstancedMesh(renderSystem, cmd, *gpuProgram))
                    {
                        gpuProgram->SetUniformMatrix("_Model", Matrix::ExpandAffine(cmd.modelMatrix), false);
                        cmd.vertexBuffer->Bind();
                        // Record composition stats
                        GraphicsProfiler::RecordMaterialDraw(material ? material->GetId() : 0, cmd.indexCount, cmd.instanceCount);
                        GraphicsProfiler::RecordProgramDraw(gpuProgram ? gpuProgram->GetHandle() : 0, cmd.indexCount, cmd.instanceCount);
                        renderSystem.DrawIndexed(cmd.indexBuffer);
                    }
                }
            }
            pass->End(renderSystem, offscreenTargets);

            rmt_EndOpenGLSample();
            rmt_EndCPUSample();
        }

        if (viewCtx.target) {
            viewCtx.target->Unbind(renderSystem);
        }
    }

    // TODO: Should be rendered after Opaque pass and before Alpha pass.
    m_skyboxRenderer->Render(renderSystem, scene);
}

bool SceneRenderer::IsSphereInside(const Frustum &viewFustrum, const Sphere &sphere, glm::mat4 modelMtx, const glm::vec3 &scale)
{
    const glm::vec3 pos = modelMtx * glm::vec4(sphere.center, 1);
    const float maxScale = std::max(std::max(scale.x, scale.y), scale.z);
    const float scaledRadius = sphere.radius * maxScale;
    return viewFustrum.IsSphereInside(pos, scaledRadius);
}
