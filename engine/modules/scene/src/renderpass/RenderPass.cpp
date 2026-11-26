#include "nexus/scene/renderpass/RenderPass.h"
#include "nexus/container/Array.h"

#include "nexus/graphics/RenderSystem.h"
#include "nexus/graphics/GpuResourceFactory.h"
#include "nexus/graphics/GraphicsProfiler.h"

#include "nexus/scene/SceneNode.h"
#include "nexus/scene/Scene.h"
#include "nexus/scene/component/CameraComponent.h"
#include "nexus/scene/component/SceneNodeComponent.h"
#include "nexus/scene/component/TransformComponent.h"
#include "nexus/scene/component/MeshComponent.h"
#include "nexus/scene/component/LightComponent.h"

#include "nexus/math/Matrix.h"

#include "nexus/memory/OwningBuffer.h"

#include "nexus/debug/Assert.h"

#include "Remotery.h"

#include <ranges>

USING_NAMESPACE_NXS;

DEFINE_LOG(RenderPass);

std::string AttachmentDesc::shadowMap = "shadowmap";

static bool RenderInstancedMesh(RenderSystem& renderSystem, const RenderCommand& command, GpuProgram& gpuProgram)
{
    rmt_ScopedCPUSample(RenderPass_RenderInstancedMesh, 0);
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

RenderPass::~RenderPass()
{
    ReleaseResources();
}

bool RenderPass::Init(const IGpuResourceFactory& factory)
{
    if (targetType == RenderTargetType::Offscreen)
    {
        // offscreenTargetName can't be invalid if the targetType is RenderTargetType::Offscreen
        NXS_ASSERT(offscreenTarget.textureDesc.IsValidDesc());
        offscreenTarget.renderTarget = factory.CreateRenderTarget(offscreenTarget.textureDesc);
    }
    return true;
}

void RenderPass::ReleaseResources()
{
    pipelineState.globalShader = nullptr;
    offscreenTarget.renderTarget.reset();
}

Array<RenderCommand> RenderPass::CreateRenderCommands(const ViewContext& viewCtx) const
{
    rmt_ScopedCPUSample(RenderPass_CreateRenderCommands, 0);
    Array<RenderCommand> commands;
    for (const auto node : viewCtx.visibleNodes)
    {
        const auto meshComp = node->GetComponent<MeshComponent>();
        if (!meshComp) continue;

        auto mesh = meshComp->GetMesh();
        if (!mesh || !IsPassFiltered(mesh)) continue;

        const auto& modelMtx = node->GetGlobalAffineTransform();
        const glm::mat4 &viewProjMtx = viewCtx.viewProjectionMatrix;
        
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
        RenderCommand cmd;
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
            commandSortType == CommandSortType::FrontToBack ? depthNormalized : 1.0f - depthNormalized);

        commands.push_back(cmd);        
    }
    return commands;
}

void RenderPass::Begin(RenderSystem& rs, SceneRenderingContext& sceneCtx)
{
    rmt_BeginCPUSampleDynamic(name.c_str(), 0);
    rmt_BeginOpenGLSampleDynamic(name.c_str());

    rmt_ScopedCPUSample(RenderPass_BeginRenderPass, 0);
    auto renderInterface = rs.GetRenderInterface();
    // NOTE: Pipeline state should be applied before anything.
    renderInterface->ApplyPipelineState(pipelineState);

    renderInterface->DebugMarker(name);
    renderInterface->SetGlobalShader(pipelineState.globalShader);

    // Only sample the designated offscreen targets
    m_sampleTargets.clear();
    for (const auto& attachment : sceneCtx.offscreenTargets)
    {
        const auto it = std::ranges::find(readTargets, attachment.name);
        if (it != readTargets.end()) {
            m_sampleTargets.push_back(attachment);
        }
    }

    Begin_Internal(rs, sceneCtx);

    // Hook
    if (onBegin) onBegin(rs, sceneCtx);
}

void RenderPass::End(RenderSystem& rs, SceneRenderingContext& sceneCtx)
{
    if (targetType == RenderTargetType::Offscreen &&
        offscreenTarget.renderTarget != nullptr)
    {
        sceneCtx.offscreenTargets.push_back(offscreenTarget);
    }
    // Reset the global shader.
    rs.GetRenderInterface()->SetGlobalShader(nullptr);
    End_Internal(rs, sceneCtx);
    // Offscreen target is unbound by the caller (RenderPass/RenderSystem).
    // Hook
    if (onEnd) onEnd(rs, sceneCtx);

    rmt_EndOpenGLSample();
    rmt_EndCPUSample();
}

void RenderPass::Render(RenderSystem& rs, const ViewContext& viewCtx, const SceneRenderingContext& sceneCtx)
{
    const Scene& scene = *sceneCtx.scene;
    Array<RenderCommand> commands = CreateRenderCommands(viewCtx);
    AppendCommands(rs, viewCtx.visibleNodes, commands);

    {
        rmt_ScopedCPUSample(RenderPass_SortMeshes, 0)
        std::ranges::sort(commands, [](const RenderCommand& a, const RenderCommand& b) {
            return a.sortKey < b.sortKey;
        });
    }

    Ref<GpuProgram> globalShader = pipelineState.globalShader;
    {
        rmt_ScopedCPUSample(RenderPass_RenderMeshes, 0)
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
                        rs.GetRenderInterface()->IsFramebufferSRGBSupported()) ? 1 : 0);
                }

                SetCameraUniforms(*gpuProgram, viewCtx.position, viewCtx.viewMatrix, viewCtx.projectionMatrix);
                SetAmbientLightParams(*gpuProgram, *scene.GetRegistry());
                SetDirectLightParams(*gpuProgram, scene, NXS_INVALID_ID);
                SetPointLightParams(*gpuProgram, scene);
                SetShadowMapParams(*gpuProgram, sceneCtx);
                usingProgram = gpuProgram;
            }

            if (!RenderInstancedMesh(rs, cmd, *gpuProgram))
            {
                gpuProgram->SetUniformMatrix("_Model", Matrix::ExpandAffine(cmd.modelMatrix), false);
                cmd.vertexBuffer->Bind();
                // Record composition stats
                GraphicsProfiler::RecordMaterialDraw(material ? material->GetId() : 0, cmd.indexCount, cmd.instanceCount);
                GraphicsProfiler::RecordProgramDraw(gpuProgram ? gpuProgram->GetHandle() : 0, cmd.indexCount, cmd.instanceCount);
                rs.DrawIndexed(cmd.indexBuffer);
            }
        }
    }
}

bool RenderPass::IsNodeActive(const SceneNodeComponent& nodeComp)
{
    const auto* node = SceneNode::FindNode(nodeComp.id);
    if (!node)
    {
        LOG_ERROR(LogRenderPass, std::format("Scene node {} was not found.", nodeComp.id));
        return false;
    }
    return node->IsActive();
}

void RenderPass::SetAmbientLightParams(GpuProgram& gpuProgram, const entt::registry& registry)
{
    rmt_ScopedCPUSample(RenderPass_SetAmbientLightParams, 0);
    const auto ambientLightEnt = registry.view<AmbientLightComponent>().front();
    const auto color = ambientLightEnt == entt::null ? Color3F::Grey : registry.get<AmbientLightComponent>(ambientLightEnt).color;
    gpuProgram.SetUniformVector("_AmbientLight", color);
}

void RenderPass::SetDirectLightParams(GpuProgram& gpuProgram, const SceneNode& scene, int32_t shadowLightId)
{
    rmt_ScopedCPUSample(RenderPass_SetDirectLightParams, 0);
    int32_t numLight = 0;
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
        const auto uniformLocationShadow = std::format("{}.castShadow", uniformLocation);
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
        gpuProgram.SetUniformVector(uniformLocationColor, light.properties.color);
        gpuProgram.SetUniformVector(uniformLocationDirection, glm::normalize(direction));
        gpuProgram.SetUniformFloat(uniformLocationDiffuse, light.properties.diffuseIntensity);
        gpuProgram.SetUniformFloat(uniformLocationSpecular, light.properties.specularIntensity);
        gpuProgram.SetUniformFloat(uniformLocationCutoff, light.properties.cutoffRange);
        gpuProgram.SetUniformInt(uniformLocationShadow, light.properties.canCastShadows ? 1 : 0);
        rmt_EndCPUSample();

        numLight++;
    }
    gpuProgram.SetUniformInt("_NumDirectLight", numLight);
}

void RenderPass::SetPointLightParams(GpuProgram& gpuProgram, const SceneNode& scene)
{
    rmt_ScopedCPUSample(RenderPass_SetPointLightParams, 0);
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
        const auto uniformLocationShadow = std::format("{}.castShadow", uniformLocation);
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
        gpuProgram.SetUniformInt(uniformLocationShadow, light.properties.canCastShadows ? 1 : 0);
        gpuProgram.SetUniformFloat(uniformLocationConst, light.constant);
        gpuProgram.SetUniformFloat(uniformLocationLinear, light.linear);
        gpuProgram.SetUniformFloat(uniformLocationQuad, light.quadratic);
        rmt_EndCPUSample();

        numLight++;
    }
    gpuProgram.SetUniformInt("_NumPointLight", numLight);
}

void RenderPass::SetCameraUniforms(GpuProgram& gpuProgram, const glm::vec3& cameraPos, const glm::mat4& viewMtx, const glm::mat4& projectionMtx)
{
    rmt_ScopedCPUSample(RenderPass_SetCameraUniforms, 0);
    gpuProgram.SetUniformVector("_CameraPos", cameraPos);
    gpuProgram.SetUniformMatrix("_View", viewMtx, false);
    gpuProgram.SetUniformMatrix("_Projection", projectionMtx, false);
}

void RenderPass::SetShadowMapParams(GpuProgram& gpuProgram, const SceneRenderingContext& sceneCtx)
{
    rmt_ScopedCPUSample(RenderPass_SetShadowMapParams, 0);
    const bool hasShadowMap = !sceneCtx.shadowMaps.empty() ;
    gpuProgram.SetUniformInt("_HasShadowMap", hasShadowMap ? 1 : 0);
    if (!hasShadowMap) 
    {
        return;
    }

    for (const auto& shadowMap : sceneCtx.shadowMaps)
    {
        // TODO: Support multiple light sources
        gpuProgram.SetUniformMatrix("_LightSpaceMatrix", shadowMap.lightSpaceMatrix, false);
        gpuProgram.SetUniformTexture2D(
            "_ShadowMap",
            shadowMap.depthMap,
            15);
    }
}
