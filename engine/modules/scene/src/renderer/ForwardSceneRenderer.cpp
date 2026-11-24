#include "nexus/scene/renderer/ForwardSceneRenderer.h"

#include "nexus/scene/renderpass/RenderPass.h"
#include "nexus/scene/renderpass/DepthPrePass.h"
#include "nexus/scene/renderpass/OpaquePass.h"
#include "nexus/scene/renderpass/EnvironmentPass.h"
#include "nexus/scene/renderpass/AlphaPass.h"
#include "nexus/scene/renderpass/OverlayPass.h"

USING_NAMESPACE_NXS;

ForwardSceneRenderer::ForwardSceneRenderer(const RenderSystem& renderSystem)
{
    RegisterRenderPass(RenderPassQueue::DepthFill, NewRef<DepthPrePass>());
    RegisterRenderPass(RenderPassQueue::Opaque, NewRef<OpaquePass>());
    RegisterRenderPass(RenderPassQueue::Environment, NewRef<EnvironmentPass>());
    RegisterRenderPass(RenderPassQueue::Alpha, NewRef<AlphaPass>());
    RegisterRenderPass(RenderPassQueue::Overlay, NewRef<OverlayPass>());
}
