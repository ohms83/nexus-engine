//
// Created by copilot on 11/28/2025.
//

#include "nexus/graphics/RenderPass.h"
#include "nexus/graphics/RenderSystem.h"

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
