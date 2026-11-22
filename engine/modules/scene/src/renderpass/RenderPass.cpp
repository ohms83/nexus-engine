#include "nexus/scene/renderpass/RenderPass.h"
#include "nexus/container/Array.h"

#include "nexus/graphics/RenderSystem.h"
#include "nexus/serialize/Serializer.h"

#include "nexus/scene/SceneNode.h"
#include "nexus/scene/component/CameraComponent.h"
#include "nexus/scene/component/SceneNodeComponent.h"
#include "nexus/scene/component/TransformComponent.h"
#include "nexus/scene/component/MeshComponent.h"
#include "nexus/scene/component/LightComponent.h"

#include "nexus/math/Matrix.h"

#include "nexus/debug/Assert.h"

#include "Remotery.h"

USING_NAMESPACE_NXS;

DEFINE_LOG(RenderPass);

RenderPass::~RenderPass()
{
    ReleaseResources();
}

void RenderPass::ReleaseResources()
{
    pipelineState.globalShader = nullptr;
}

void RenderPass::Begin(RenderSystem& rs) const
{
    rmt_ScopedCPUSample(RenderPass_BeginRenderPass, 0);
    auto renderInterface = rs.GetRenderInterface();
    // NOTE: Pipeline state should be applied before anything.
    renderInterface->ApplyPipelineState(pipelineState);

    renderInterface->DebugMarker(name);
    renderInterface->SetGlobalShader(pipelineState.globalShader);

    Begin_Internal(rs);

    // Hook
    if (onBegin) onBegin(rs);
}

void RenderPass::End(RenderSystem& rs) const
{
    // Reset the global shader.
    rs.GetRenderInterface()->SetGlobalShader(nullptr);
    End_Internal(rs);
    // Offscreen target is unbound by the caller (SceneRenderer/RenderSystem).
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
    // SetCameraUniforms(gpuProgram, viewCtx.position, viewCtx.viewMatrix, viewCtx.projectionMatrix);
    // SetAmbientLightParams(gpuProgram, *scene.GetRegistry());
    // SetDirectLightParams(gpuProgram, scene, m_shadowMapReady ? m_shadowLightId : NXS_INVALID_ID);
    // SetPointLightParams(gpuProgram, scene);
    // SetShadowMapParams(gpuProgram);
}

void RenderPass::SetAmbientLightParams(Ref<GpuProgram> gpuProgram, const entt::registry& registry)
{
    
}

void RenderPass::SetDirectLightParams(Ref<GpuProgram> gpuProgram, const SceneNode& scene, int32_t shadowLightId)
{
    
}

void RenderPass::SetPointLightParams(Ref<GpuProgram> gpuProgram, const SceneNode& scene)
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
            LOG_ERROR(LogRenderPass, std::format("SceneRenderer::SetPointLightParams: SceneNode with ID {} not found.", node.id));
            rmt_EndCPUSample();
            continue;
        }
        gpuProgram->SetUniformVector(uniformLocationPosition, nodePtr->GetGlobalPosition());
        gpuProgram->SetUniformVector(uniformLocationColor, light.properties.color);
        gpuProgram->SetUniformFloat(uniformLocationDiffuse, light.properties.diffuseIntensity);
        gpuProgram->SetUniformFloat(uniformLocationSpecular, light.properties.specularIntensity);
        gpuProgram->SetUniformFloat(uniformLocationCutOff, light.properties.cutoffRange);
        gpuProgram->SetUniformFloat(uniformLocationConst, light.constant);
        gpuProgram->SetUniformFloat(uniformLocationLinear, light.linear);
        gpuProgram->SetUniformFloat(uniformLocationQuad, light.quadratic);
        rmt_EndCPUSample();

        numLight++;
    }
    gpuProgram->SetUniformInt("_NumPointLight", numLight);
}

void RenderPass::SetCameraUniforms(Ref<GpuProgram> gpuProgram, const glm::vec3& cameraPos, const glm::mat4& viewMtx, const glm::mat4& projectionMtx)
{
    
}

VariantData RenderPass::Serialize() const
{
    VariantData::Map data;
    data["name"] = name;
    data["enabled"] = enabled;
    data["targetType"] = INT_CAST(targetType);

    // Pipeline
    VariantData::Map pipeline;
    pipeline["depthTest"] = pipelineState.depthTest;
    pipeline["depthWrite"] = pipelineState.depthWrite;
    pipeline["depthFunction"] = INT_CAST(pipelineState.depthFunction);
    pipeline["stencilTest"] = pipelineState.stencilTest;
    pipeline["stencilOp"] = INT_CAST(pipelineState.stencilOp);
    pipeline["overrideBlendMode"] = INT_CAST(pipelineState.overrideBlendMode);
    pipeline["polygonMode"] = INT_CAST(pipelineState.polygonMode);
    pipeline["cullMode"] = INT_CAST(pipelineState.cullMode);
    pipeline["frontFace"] = INT_CAST(pipelineState.frontFace);
    data["pipelineState"] = pipeline;

    // Shader
    data["globalShaderName"] = globalShaderName;
    VariantData::Map shaderData;
    shaderData["vertex"] = globalShaderSources.find(GpuProgram::Type::Vertex) != globalShaderSources.end() ? globalShaderSources.at(GpuProgram::Type::Vertex) : "";
    shaderData["fragment"] = globalShaderSources.find(GpuProgram::Type::Fragment) != globalShaderSources.end() ? globalShaderSources.at(GpuProgram::Type::Fragment) : "";
    shaderData["geometry"] = globalShaderSources.find(GpuProgram::Type::Geometry) != globalShaderSources.end() ? globalShaderSources.at(GpuProgram::Type::Geometry) : "";
    data["globalShader"] = shaderData;

    // Attachments
    VariantData::Array attachArr;
    for (const auto& a : attachments)
    {
        attachArr.emplace_back(VariantData::Map{
            {"format", INT_CAST(a.format)},
            {"load", a.load},
            {"store", a.store},
            {"clear", a.clear},
            {"samples", INT_CAST(a.samples)}
        });
    }
    data["attachments"] = attachArr;

    data["layerMask"] = INT_CAST(layerMask);
    data["filterType"] = filterType;
    data["offscreenTargetName"] = offscreenTargetName;
    VariantData::Array readArr;
    for (const auto& r : readTargets) readArr.emplace_back(r);
    data["readTargets"] = readArr;

    return data;
}

bool RenderPass::Deserialize(const VariantData& data)
{
    name = data["name"].GetString();
    enabled = data["enabled"].GetBool();
    targetType = CAST<RenderTargetType>(data["targetType"].GetInt());

    // Pipeline
    const auto& p = data["pipelineState"].GetMap();
    pipelineState.depthTest = p.at("depthTest").GetBool();
    pipelineState.depthWrite = p.at("depthWrite").GetBool();
    pipelineState.depthFunction = CAST<DepthFunction>(p.at("depthFunction").GetInt());
    pipelineState.stencilTest = p.at("stencilTest").GetBool();
    pipelineState.stencilOp = CAST<StencilOperation>(p.at("stencilOp").GetInt());
    pipelineState.overrideBlendMode = CAST<BlendMode>(p.at("overrideBlendMode").GetInt());
    pipelineState.polygonMode = CAST<PolygonMode>(p.at("polygonMode").GetInt());
    pipelineState.cullMode = CAST<PolygonFacing>(p.at("cullMode").GetInt());
    pipelineState.frontFace = CAST<FrontFace>(p.at("frontFace").GetInt());
    
    // Shader
    globalShaderName = data["globalShaderName"].GetString();
    const auto& s = data["globalShader"].GetMap();
    globalShaderSources[GpuProgram::Type::Vertex] = s.at("vertex").GetString();
    globalShaderSources[GpuProgram::Type::Fragment] = s.at("fragment").GetString();
    globalShaderSources[GpuProgram::Type::Geometry] = s.at("geometry").GetString();

    // attachments
    attachments.clear();
    if (data["attachments"].IsArray())
    {
        for (const auto& av : data["attachments"].GetArray())
        {
            const auto& am = av.GetMap();
            AttachmentDesc a;
            a.format = CAST<PixelFormat>(am.at("format").GetInt());
            a.load = am.at("load").GetBool();
            a.store = am.at("store").GetBool();
            a.clear = am.at("clear").GetBool();
            a.samples = UINT_CAST(am.at("samples").GetInt());
            attachments.push_back(a);
        }
    }

    layerMask = UINT_CAST(data["layerMask"].GetInt());
    filterType = data["filterType"].GetString();
    offscreenTargetName = data["offscreenTargetName"].GetString();
    readTargets.clear();
    if (data.HasKey("readTargets") && data["readTargets"].IsArray())
    {
        for (const auto& rv : data["readTargets"].GetArray())
        {
            readTargets.emplace_back(rv.GetString());
        }
    }

    // Restore filter based on filterType preset
    SetFilterType(filterType);
    return true;
}

void RenderPass::Resolve(RenderingInterface &renderingInterface)
{
    if (IsResolved()) return;

    if (!globalShaderSources.empty())
    {    
        Hasher hasher;
        auto shader = std::make_shared<Shader>(globalShaderName, hasher.Hash32(name));
        if (shader->CompileFromSource(
            globalShaderSources[GpuProgram::Type::Vertex],
            globalShaderSources[GpuProgram::Type::Fragment],
            globalShaderSources[GpuProgram::Type::Geometry]))
        {
            pipelineState.globalShader = shader->GetGpuProgram();
        }
    }
}

void RenderPass::SetFilterType(std::string type)
{
    filterType = std::move(type);
    if (filterType == "opaque")
    {
        filter = [](const RenderCommand& cmd) {
            return cmd.material && cmd.material->blendMode == BlendMode::None;
        };
    }
    else if (filterType == "alpha")
    {
        filter = [](const RenderCommand& cmd) {
            return cmd.material && cmd.material->blendMode != BlendMode::None;
        };
    }
    else if (filterType == "overlay")
    {
        filter = [](const RenderCommand& cmd) {
            return cmd.material &&
                   cmd.material->blendMode != BlendMode::None &&
                   !cmd.material->depthWrite &&
                   cmd.material->depthFunction == DepthFunction::Always;
        };
    }
    else // "all" or unknown
    {
        filter = nullptr;
    }
}
