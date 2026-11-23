#pragma once

#include "nexus/scene/renderpass/RenderPass.h"

NXS_NAMESPACE
{
    class DepthPrePass : public RenderPass
    {
    public:
        DepthPrePass();

        bool Init(const IGpuResourceFactory& factory) override;

        bool IsPassFiltered(Ref<const Mesh> mesh) const override
        {
            return mesh->GetMaterial() && mesh->GetMaterial()->blendMode == BlendMode::None;
        }
    };
}