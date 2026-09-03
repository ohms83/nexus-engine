#pragma once

#include "nexus/scene/RenderPass.h"

NXS_NAMESPACE
{
    class OpaquePass : public RenderPass
    {
    public:
        OpaquePass()
        {
            name = "Opaque Pass";
            priority = RENDER_PASS_OPAQUE;
            clearFlags = ClearFlags::Color | ClearFlags::Depth;
            clearColor = Color3F(0x303030ff);
            clearDepth = 1.0f;
            pipelineState.depthTest = true;
            pipelineState.depthWrite = true;
            filterType = "opaque";
        }

        void PopulateSceneView(const SceneNode& scene, std::vector<Ref<SceneView>>& outViews) const override;
        void PrepareRenderCommands(const SceneNode& scene, const SceneView& view, std::vector<RenderCommand>& outCommands) const override;
    };
}