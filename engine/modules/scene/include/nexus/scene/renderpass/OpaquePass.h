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
            auto material = mesh->GetMaterial();
            return material && material->blendMode == BlendMode::None;
        }
    };
}