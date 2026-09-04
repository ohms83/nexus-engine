#include "nexus/scene/renderpass/OpaquePass.h"

USING_NAMESPACE_NXS;

OpaquePass::OpaquePass()
{
    name = "Opaque Pass";
    priority = RENDER_PASS_OPAQUE;
    commandSortType = CommandSortType::FrontToBack;
    clearFlags = ClearFlags::Color | ClearFlags::Depth;
    clearColor = Color3F(0x303030ff);
    clearDepth = 1.0f;
    pipelineState.depthTest = true;
    pipelineState.depthWrite = true;
    filterType = "opaque";
}