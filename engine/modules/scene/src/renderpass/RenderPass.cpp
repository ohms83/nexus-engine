#include "nexus/scene/renderpass/RenderPass.h"
#include "nexus/container/Array.h"

#include "nexus/graphics/RenderSystem.h"
#include "nexus/graphics/GpuResourceFactory.h"

#include "nexus/scene/SceneNode.h"
#include "nexus/scene/component/CameraComponent.h"
#include "nexus/scene/component/SceneNodeComponent.h"
#include "nexus/scene/component/TransformComponent.h"
#include "nexus/scene/component/MeshComponent.h"
#include "nexus/scene/component/LightComponent.h"

#include "nexus/math/Matrix.h"

#include "nexus/debug/Assert.h"

#include "Remotery.h"

#include <ranges>

USING_NAMESPACE_NXS;

DEFINE_LOG(RenderPass);

std::string AttachmentDesc::shadowMap = "shadowmap";

RenderPass::~RenderPass()
{
    ReleaseResources();
}

bool RenderPass::Init()
{
    if (targetType == RenderTargetType::Offscreen)
    {
        // offscreenTargetName can't be invalid if the targetType is RenderTargetType::Offscreen
        NXS_ASSERT(offscreenTarget.IsValidDesc());
        offscreenTarget.renderTarget = GetGpuResourceFactory()->CreateRenderTarget(
            offscreenTarget.widht,
            offscreenTarget.height,
            true
        );
    }
    return true;
}

void RenderPass::ReleaseResources()
{
    pipelineState.globalShader = nullptr;
    offscreenTarget.renderTarget.reset();
}

void RenderPass::Begin(RenderSystem& rs, const Array<AttachmentDesc>& sampleTargets)
{
    rmt_ScopedCPUSample(RenderPass_BeginRenderPass, 0);
    auto renderInterface = rs.GetRenderInterface();
    // NOTE: Pipeline state should be applied before anything.
    renderInterface->ApplyPipelineState(pipelineState);

    renderInterface->DebugMarker(name);
    renderInterface->SetGlobalShader(pipelineState.globalShader);

    // Only sample the designated offscreen targets
    m_sampleTargets.clear();
    for (const auto& attachment : sampleTargets)
    {
        const auto it = std::ranges::find(readTargets, attachment.name);
        if (it != readTargets.end()) {
            m_sampleTargets.push_back(attachment);
        }
    }

    Begin_Internal(rs);

    // Hook
    if (onBegin) onBegin(rs);
}

void RenderPass::End(RenderSystem& rs, Array<AttachmentDesc>& outTargets)
{
    if (targetType == RenderTargetType::Offscreen &&
        offscreenTarget.renderTarget != nullptr)
    {
        outTargets.push_back(offscreenTarget);
    }
    // Reset the global shader.
    rs.GetRenderInterface()->SetGlobalShader(nullptr);
    End_Internal(rs);
    // Offscreen target is unbound by the caller (RenderPass/RenderSystem).
    // Hook
    if (onEnd) onEnd(rs);
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

void RenderPass::SetShaderParams(GpuProgram& gpuProgram, const SceneNode& scene, const ViewContext& viewCtx) const
{
    SetCameraUniforms(gpuProgram, viewCtx.position, viewCtx.viewMatrix, viewCtx.projectionMatrix);
    SetAmbientLightParams(gpuProgram, *scene.GetRegistry());
    SetDirectLightParams(gpuProgram, scene, shadowLightId);
    SetPointLightParams(gpuProgram, scene);
    // SetShadowMapParams(gpuProgram);
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
            LOG_ERROR(LogRenderPass, std::format("RenderPass::SetDirectLightParams: SceneNode with ID {} not found.", node.id));
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
        const auto uniformLocationPosition = std::format("{}.position", uniformLocation);
        const auto uniformLocationConst = std::format("{}.constant", uniformLocation);
        const auto uniformLocationLinear = std::format("{}.linear", uniformLocation);
        const auto uniformLocationQuad = std::format("{}.quadratic", uniformLocation);
        rmt_EndCPUSample();

        rmt_BeginCPUSample(EmplaceUniformValues, 0);
        SceneNode* nodePtr = SceneNode::FindNode(node.id);
        if (!nodePtr) {
            LOG_ERROR(LogRenderPass, std::format("RenderPass::SetPointLightParams: SceneNode with ID {} not found.", node.id));
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

void RenderPass::SetCameraUniforms(GpuProgram& gpuProgram, const glm::vec3& cameraPos, const glm::mat4& viewMtx, const glm::mat4& projectionMtx)
{
    rmt_ScopedCPUSample(RenderPass_SetCameraUniforms, 0);
    gpuProgram.SetUniformVector("_CameraPos", cameraPos);
    gpuProgram.SetUniformMatrix("_View", viewMtx, false);
    gpuProgram.SetUniformMatrix("_Projection", projectionMtx, false);
}

void RenderPass::SetShadowMapParams(GpuProgram& gpuProgram)
{
}
