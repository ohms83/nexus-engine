#include "nexus/scene/renderpass/DepthPrePass.h"
#include "nexus/debug/LogDispatcher.h"

USING_NAMESPACE_NXS;

DEFINE_LOG(DepthPrePass);

// Shader sources
static const char* s_depthVertexShader = R"(
#version 330 core
layout (location = 0) in vec3 aPos;

invariant gl_Position;
uniform mat4 _Model;
uniform mat4 _View;
uniform mat4 _Projection;

void main()
{
    vec4 worldPos = _Model * vec4(aPos, 1.0);
    gl_Position = _Projection * _View * worldPos;
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
    pipelineState.colorMask = glm::bvec4(false);
    pipelineState.overrideBlendMode = BlendMode::None;
    globalShaderName = "_DepthShader";
    globalShaderSources = {
        { GpuProgram::Type::Vertex, s_depthVertexShader },
        { GpuProgram::Type::Fragment, s_depthFragmentShader }
    };
    filterType = "opaque";
}

bool DepthPrePass::Init()
{
    const auto* factory = GetGpuResourceFactory();
    if (!factory)
    {
        LOG_ERROR(LogDepthPrePass, "Cannot initialize the depth pre-pass without a GPU resource factory.");
        return false;
    }
    return Init(*factory);
}

bool DepthPrePass::Init(const IGpuResourceFactory& factory)
{
    if (pipelineState.globalShader) return true;

    const auto vertex = globalShaderSources.find(GpuProgram::Type::Vertex);
    const auto fragment = globalShaderSources.find(GpuProgram::Type::Fragment);
    if (vertex == globalShaderSources.end() || vertex->second.empty() ||
        fragment == globalShaderSources.end() || fragment->second.empty())
    {
        LOG_ERROR(LogDepthPrePass, "The depth pre-pass requires vertex and fragment shader sources.");
        return false;
    }

    Shader shader(globalShaderName, 0);
    if (!shader.CompileFromSource(factory, vertex->second, fragment->second, ""))
    {
        LOG_ERROR(LogDepthPrePass, "Failed to compile the depth pre-pass shader.");
        return false;
    }
    pipelineState.globalShader = shader.GetGpuProgram();
    return true;
}