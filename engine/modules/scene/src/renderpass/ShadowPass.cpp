#include "nexus/scene/renderpass/ShadowPass.h"

USING_NAMESPACE_NXS;

DEFINE_LOG(ShadowPass);

ShadowPass::ShadowPass()
{
    name = "Shadow Pass";
    targetType = RenderTargetType::Offscreen;
    offscreenTarget = AttachmentDesc {
        .name = AttachmentDesc::shadowMap,
        .textureDesc = TextureDescription::DepthTextureDesc(2048, 2048),
    };

    commandSortType = CommandSortType::FrontToBack;
    pipelineState.depthTest = true;
    pipelineState.depthWrite = true;
    pipelineState.depthFunction = DepthFunction::LesserOrEqual;
}

bool ShadowPass::Init(const IGpuResourceFactory& factory)
{
    if (!RenderPass::Init(factory))
    {
        enabled = false;
        return false;
    }
    if (!offscreenTarget.IsValid() || offscreenTarget.name != AttachmentDesc::shadowMap)
    {
        LOG_ERROR(LogShadowPass, "Failed to create a directional shadow map render target.");
        enabled = false;
        return false;
    }
    auto shader = std::make_shared<Shader>("_DirectionalShadowMap", 0);
    constexpr auto vertexSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        uniform mat4 _Model;
        uniform mat4 _LightSpaceMatrix;
        void main()
        {
            gl_Position = _LightSpaceMatrix * _Model * vec4(aPos, 1.0);
        }
    )";
    constexpr auto fragmentSource = R"(
        #version 330 core
        void main() {}
    )";
    if (!shader->CompileFromSource(factory, vertexSource, fragmentSource, ""))
    {
        LOG_ERROR(LogShadowPass, "Failed to compile the directional shadow map shader.");
        return false;
    }
    pipelineState.globalShader = shader->GetGpuProgram();
    return true;
}

void ShadowPass::Begin_Internal(RenderSystem& rs)
{
}