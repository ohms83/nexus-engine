#pragma once

#include "nexus/scene/renderpass/RenderPass.h"

NXS_NAMESPACE
{
    class OpaquePass : public RenderPass
    {
    public:
        OpaquePass();

        bool IsPassFiltered(Ref<const Mesh> mesh) const override
        {
            return mesh->GetMaterial() && mesh->GetMaterial()->blendMode == BlendMode::None;
        }
    };
}