#include "nexus/scene/renderer/ForwardSceneRenderer.h"

#include "nexus/scene/renderpass/RenderPass.h"
#include "nexus/scene/renderpass/DepthPrePass.h"
#include "nexus/scene/renderpass/OpaquePass.h"
#include "nexus/scene/renderpass/AlphaPass.h"
#include "nexus/scene/renderpass/OverlayPass.h"

USING_NAMESPACE_NXS;

ForwardSceneRenderer::ForwardSceneRenderer(const RenderSystem& renderSystem)
{
    // RegisterRenderPass(NewRef<DepthPrePass>());
    RegisterRenderPass(NewRef<OpaquePass>());
    RegisterRenderPass(NewRef<AlphaPass>());
    RegisterRenderPass(NewRef<OverlayPass>());
    SortRenderPasses();
}
