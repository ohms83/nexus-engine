#include "nexus/scene/renderpass/DepthPrePass.h"

USING_NAMESPACE_NXS;

// Shader sources
static const char* s_depthVertexShader = R"(
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 _Model;
uniform mat4 _View;
uniform mat4 _Projection;

void main()
{
    gl_Position = _Projection * _View * _Model * vec4(aPos, 1.0);
    gl_Position.z += 0.001;
}
)";

static const char* s_depthFragmentShader = R"(
#version 330 core
void main()
{
    // Do nothing. This shader outputs no color (gl_FragColor is not written), 
    // but the depth value determined by gl_Position is still written to the depth buffer.
}
)";

DepthPrePass::DepthPrePass()
{
    name = "Depth Pre-Pass";
    priority = RENDER_PASS_DEPTH_FILL;
    commandSortType = CommandSortType::FrontToBack;
    clearFlags = ClearFlags::Depth;
    clearDepth = 1.0f;
    pipelineState.depthTest = true;
    pipelineState.depthWrite = true;
    pipelineState.overrideBlendMode = BlendMode::None;
    globalShaderName = "_DepthShader";
    globalShaderSources = {
        { GpuProgram::Type::Vertex, s_depthVertexShader },
        { GpuProgram::Type::Fragment, s_depthFragmentShader }
    };
    filterType = "opaque";
}