#include "nexus/scene/renderpass/OpaquePass.h"

USING_NAMESPACE_NXS;

OpaquePass::OpaquePass()
{
    name = "Opaque Pass";
    commandSortType = CommandSortType::FrontToBack;
    pipelineState.depthTest = true;
    pipelineState.depthWrite = true;
    pipelineState.depthFunction = DepthFunction::LesserOrEqual;
}