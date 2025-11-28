//
// Created by copilot on 11/28/2025.
//

#include "nexus/graphics/RenderPass.h"
#include "nexus/graphics/RenderSystem.h"
#include "nexus/io/Serializer.h"

USING_NAMESPACE_NXS;

void RenderPass::Begin(RenderSystem& rs) const
{
    auto renderInterface = rs.GetRenderInterface();
    // Clear
    auto hasFlag = [](ClearFlags flags, ClearFlags f){ return (static_cast<uint32_t>(flags) & static_cast<uint32_t>(f)) != 0; };
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

    // Pipeline state
    renderInterface->SetDepthFunction(pipelineState.depthFunction);
    renderInterface->SetDepthMask(pipelineState.depthWrite);
    renderInterface->SetCullMode(pipelineState.cullMode);
    renderInterface->SetFrontFace(pipelineState.frontFace);
    renderInterface->SetPolygonMode(pipelineState.polygonMode);

    // Global shader override
    if (globalShader)
    {
        renderInterface->SetGlobalShader(globalShader->GetGpuProgram());
    }
    else
    {
        renderInterface->SetGlobalShader(nullptr);
    }

    // Hook
    if (onBegin) onBegin(rs);
}

void RenderPass::End(RenderSystem& rs) const
{
    auto renderInterface = rs.GetRenderInterface();
    // Reset the global shader.
    renderInterface->SetGlobalShader(nullptr);
    // Hook
    if (onEnd) onEnd(rs);
}

VariantData RenderPass::Serialize() const
{
    VariantData::Map data;
    data["name"] = name;
    data["priority"] = INT_CAST(priority);
    data["enabled"] = enabled;
    data["targetType"] = INT_CAST(static_cast<int>(targetType));
    data["clearFlags"] = INT_CAST(static_cast<int>(clearFlags));
    data["clearColor"] = VariantData::Map {
        {"r", static_cast<double>(clearColor.r)},
        {"g", static_cast<double>(clearColor.g)},
        {"b", static_cast<double>(clearColor.b)},
    };
    data["clearDepth"] = static_cast<double>(clearDepth);
    data["clearStencil"] = INT_CAST(clearStencil);

    // Pipeline
    VariantData::Map pipeline;
    pipeline["depthTest"] = pipelineState.depthTest;
    pipeline["depthWrite"] = pipelineState.depthWrite;
    pipeline["depthFunction"] = INT_CAST(static_cast<int>(pipelineState.depthFunction));
    pipeline["stencilTest"] = pipelineState.stencilTest;
    pipeline["stencilOp"] = INT_CAST(static_cast<int>(pipelineState.stencilOp));
    pipeline["cullBackFaces"] = pipelineState.cullBackFaces;
    pipeline["overrideBlendMode"] = INT_CAST(static_cast<int>(pipelineState.overrideBlendMode));
    pipeline["polygonMode"] = INT_CAST(static_cast<int>(pipelineState.polygonMode));
    pipeline["cullMode"] = INT_CAST(static_cast<int>(pipelineState.cullMode));
    pipeline["frontFace"] = INT_CAST(static_cast<int>(pipelineState.frontFace));
    data["pipelineState"] = pipeline;

    // Shader
    data["globalShader"] = globalShader ? globalShader->GetPath() : std::string();

    // Attachments
    VariantData::Array attachArr;
    for (const auto& a : attachments)
    {
        attachArr.emplace_back(VariantData::Map{
            {"format", INT_CAST(static_cast<int>(a.format))},
            {"load", a.load},
            {"store", a.store},
            {"clear", a.clear},
            {"samples", INT_CAST(a.samples)}
        });
    }
    data["attachments"] = attachArr;

    data["layerMask"] = INT_CAST(layerMask);
    data["filterType"] = filterType;

    return data;
}

void RenderPass::Deserialize(const VariantData& data)
{
    name = data["name"].GetString();
    priority = UINT_CAST(data["priority"].GetInt());
    enabled = data["enabled"].GetBool();
    targetType = static_cast<RenderTargetType>(data["targetType"].GetInt());
    clearFlags = static_cast<ClearFlags>(data["clearFlags"].GetInt());
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
    pipelineState.depthFunction = static_cast<DepthFunction>(p.at("depthFunction").GetInt());
    pipelineState.stencilTest = p.at("stencilTest").GetBool();
    pipelineState.stencilOp = static_cast<StencilOperation>(p.at("stencilOp").GetInt());
    pipelineState.cullBackFaces = p.at("cullBackFaces").GetBool();
    pipelineState.overrideBlendMode = static_cast<BlendMode>(p.at("overrideBlendMode").GetInt());
    pipelineState.polygonMode = static_cast<PolygonMode>(p.at("polygonMode").GetInt());
    pipelineState.cullMode = static_cast<PolygonFacing>(p.at("cullMode").GetInt());
    pipelineState.frontFace = static_cast<FrontFace>(p.at("frontFace").GetInt());

    // global shader path, do not try to auto-load here (resource managers/callers may handle)
    const auto shaderPath = data["globalShader"].GetString();
    if (!shaderPath.empty())
    {
        // Defer loading to the user: store path in a temporary shader
        // For now, we set globalShader to nullptr and leave resolution to the caller.
        globalShader = nullptr;
    }

    // attachments
    attachments.clear();
    if (data["attachments"].IsArray())
    {
        for (const auto& av : data["attachments"].GetArray())
        {
            const auto& am = av.GetMap();
            AttachmentDesc a;
            a.format = static_cast<PixelFormat>(am.at("format").GetInt());
            a.load = am.at("load").GetBool();
            a.store = am.at("store").GetBool();
            a.clear = am.at("clear").GetBool();
            a.samples = static_cast<uint32_t>(am.at("samples").GetInt());
            attachments.push_back(a);
        }
    }

    layerMask = UINT_CAST(data["layerMask"].GetInt());
    filterType = data["filterType"].GetString();

    // Restore filter based on filterType preset
    if (filterType == "opaque")
    {
        filter = [](const Material& m){ return m.blendMode == BlendMode::None; };
    }
    else if (filterType == "alpha")
    {
        filter = [](const Material& m){ return m.blendMode != BlendMode::None; };
    }
    else
    {
        filter = nullptr;
    }
}

