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
            commandSortType = CommandSortType::BackToFront;
            pipelineState.depthTest = true;
            pipelineState.depthWrite = true;
            pipelineState.overrideBlendMode = BlendMode::Alpha;
        }

        bool IsPassFiltered(Ref<const Mesh> mesh) const override
        {
            return mesh->GetMaterial() && mesh->GetMaterial()->blendMode != BlendMode::None;
        }
    };
}