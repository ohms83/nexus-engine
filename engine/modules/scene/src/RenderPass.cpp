#include "nexus/scene/RenderPass.h"
#include "nexus/scene/renderpass/OpaquePass.h"

#include "nexus/graphics/RenderSystem.h"
#include "nexus/serialize/Serializer.h"

USING_NAMESPACE_NXS;

// Shader sources
static const char* s_depthVertexShader = R"(
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 _Model;
uniform mat4 _View;
uniform mat4 _Projection;

void main()
{
    gl_Position = _Projection * _View * _Model * vec4(aPos, 1.0);
    gl_Position.z += 0.1;
}
)";

static const char* s_depthFragmentShader = R"(
#version 330 core
void main()
{
    // Do nothing. This shader outputs no color (gl_FragColor is not written), 
    // but the depth value determined by gl_Position is still written to the depth buffer.
}
)";

NXS_NAMESPACE
{
    RenderPass DepthPrepass = RenderPassBuilder::Begin("Depth Prepass", RENDER_PASS_DEPTH_FILL)
        .ClearFlags(ClearFlags::Depth)
        .ClearDepth(1.0f)
        .DepthTest(true)
        .DepthWrite(true)
        .GlobalShaderName("_DepthShader")
        .GlobalShaderSources({
            { GpuProgram::Type::Vertex, s_depthVertexShader },
            { GpuProgram::Type::Fragment, s_depthFragmentShader }
        })
        .FilterType("opaque")
        .Build();

    // RenderPass OpaquePass = RenderPassBuilder::Begin("Opaque Pass", RENDER_PASS_OPAQUE)
    //     .ClearFlags(ClearFlags::Color | ClearFlags::Depth)
    //     .ClearColor(Color3F(0x303030ff))
    //     .ClearDepth(1.0f)
    //     .DepthTest(true)
    //     .DepthWrite(true)
    //     .GlobalShader(nullptr) // Use material shaders
    //     .FilterType("opaque")
    //     .Build();

    RenderPass AlphaPass = RenderPassBuilder::Begin("Alpha Pass", RENDER_PASS_ALPHA)
        .ClearFlags(ClearFlags::None)
        .DepthTest(true)
        .DepthWrite(true)
        .BlendMode(BlendMode::Alpha)
        .GlobalShader(nullptr) // Use material shaders
        .FilterType("alpha")
        .Build();

    RenderPass OverlayPass = RenderPassBuilder::Begin("Overlay Pass", RENDER_PASS_OVERLAY)
        .ClearFlags(ClearFlags::None)
        .DepthTest(false)
        .DepthWrite(false)
        .GlobalShader(nullptr) // Use material shaders
        .FilterType("overlay")
        .Build();
}

void RenderPass::ReleaseResources()
{
    pipelineState.globalShader = nullptr;
}

void RenderPass::Begin(RenderSystem& rs) const
{
    // Pipeline state using RenderSystem caching to avoid redundant state flips
    // NOTE: Pipeline state should be applied before anything.
    rs.ApplyPipelineState(pipelineState);

    auto renderInterface = rs.GetRenderInterface();
    renderInterface->DebugMarker(name);
    // Offscreen target is bound by the caller (SceneRenderer/RenderSystem).
    // Clear
    auto hasFlag = [](ClearFlags flags, ClearFlags f){ return (UINT_CAST(flags) & UINT_CAST(f)) != 0; };
    if (hasFlag(clearFlags, ClearFlags::Color) && hasFlag(clearFlags, ClearFlags::Depth))
    {
        renderInterface->ClearBuffer(Color4F{clearColor.r, clearColor.g, clearColor.b, 1.0f}, clearDepth);
    }
    else
    {
        if (hasFlag(clearFlags, ClearFlags::Color))
        {
            renderInterface->ClearColor(Color4F{clearColor.r, clearColor.g, clearColor.b, 1.0f});
        }
        if (hasFlag(clearFlags, ClearFlags::Depth))
        {
            renderInterface->ClearDepth(clearDepth);
        }
        // Stencil: Not supported yet by the generic interface. Add later if needed.
    }

    // Set global shader override via RenderSystem to allow caching
    rs.SetGlobalShader(pipelineState.globalShader ? pipelineState.globalShader : nullptr);

    // Hook
    if (onBegin) onBegin(rs);
}

void RenderPass::End(RenderSystem& rs) const
{
    // Reset the global shader.
    rs.SetGlobalShader(nullptr);
    // Offscreen target is unbound by the caller (SceneRenderer/RenderSystem).
    // Hook
    if (onEnd) onEnd(rs);
}

VariantData RenderPass::Serialize() const
{
    VariantData::Map data;
    data["name"] = name;
    data["priority"] = INT_CAST(priority);
    data["enabled"] = enabled;
    data["targetType"] = INT_CAST(targetType);
    data["clearFlags"] = INT_CAST(clearFlags);
    data["clearColor"] = VariantData::Map {
        {"r", DOUBLE_CAST(clearColor.r)},
        {"g", DOUBLE_CAST(clearColor.g)},
        {"b", DOUBLE_CAST(clearColor.b)},
    };
    data["clearDepth"] = DOUBLE_CAST(clearDepth);
    data["clearStencil"] = INT_CAST(clearStencil);

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
    priority = UINT_CAST(data["priority"].GetInt());
    enabled = data["enabled"].GetBool();
    targetType = CAST<RenderTargetType>(data["targetType"].GetInt());
    clearFlags = CAST<ClearFlags>(data["clearFlags"].GetInt());
    const auto& cc = data["clearColor"].GetMap();
    clearColor.r = FLOAT_CAST(cc.at("r").GetDouble());
    clearColor.g = FLOAT_CAST(cc.at("g").GetDouble());
    clearColor.b = FLOAT_CAST(cc.at("b").GetDouble());
    clearDepth = FLOAT_CAST(data["clearDepth"].GetDouble());
    clearStencil = UINT_CAST(data["clearStencil"].GetInt());

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
        if (shader->CompileFromSource(renderingInterface,
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

RenderCommand RenderPass::CreateRenderCommand(Ref<const Mesh> mesh, const glm::mat4 &modelMtx, const glm::mat4 &mvpMtx) const
{
    const auto material = mesh->GetMaterial();
    const auto meshSphere = mesh->GetSphere();
    const auto pos = mvpMtx * glm::vec4(meshSphere.center, 1);
    const auto clipZ = pos.z / pos.w;
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
    cmd.bounds = mesh->GetSphere();
    cmd.layerMask = 0xFFFFFFFFu;
    cmd.material = material;
    cmd.SetSortKey(
        translucent,
        materialId,
        commandSortType == CommandSortType::FrontToBack ? depthNormalized : 1.0f - depthNormalized);
    return cmd;
}
