#pragma once

#include "nexus/scene/renderpass/RenderPass.h"

NXS_NAMESPACE
{
    class OverlayPass : public RenderPass
    {
    public:
        OverlayPass()
        {
            name = "Overlay Pass";
            priority = RENDER_PASS_OVERLAY;
            commandSortType = CommandSortType::FrontToBack;
            clearFlags = ClearFlags::None;
            pipelineState.depthTest = false;
            pipelineState.depthWrite = false;
            pipelineState.overrideBlendMode = BlendMode::Alpha;
            filterType = "overlay";
        }

        bool IsPassFiltered(Ref<const Mesh> mesh) const override
        {
            const auto material = mesh->GetMaterial();
            return material &&
                   material->blendMode != BlendMode::None &&
                   !material->depthWrite &&
                   material->depthFunction == DepthFunction::Always;;
        }
    };
}