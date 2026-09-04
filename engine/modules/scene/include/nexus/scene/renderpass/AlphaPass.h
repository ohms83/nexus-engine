#pragma once

#include "nexus/scene/renderpass/RenderPass.h"

NXS_NAMESPACE
{
    class AlphaPass : public RenderPass
    {
    public:
        AlphaPass()
        {
            name = "Alpha Pass";
            priority = RENDER_PASS_ALPHA;
            commandSortType = CommandSortType::BackToFront;
            clearFlags = ClearFlags::None;
            clearColor = Color3F(0x303030ff);
            clearDepth = 1.0f;
            pipelineState.depthTest = true;
            pipelineState.depthWrite = true;
            pipelineState.overrideBlendMode = BlendMode::Alpha;
            filterType = "alpha";
        }

        bool IsPassFiltered(Ref<const Mesh> mesh) const override
        {
            return mesh->GetMaterial() && mesh->GetMaterial()->blendMode != BlendMode::None;
        }
    };
}