#include "nexus/scene/renderpass/ShadowPass.h"

USING_NAMESPACE_NXS;

DEFINE_LOG(ShadowPass);

bool ShadowPass::Init()
{
    m_shadowMapReady = false;
    // TODO: Make shadow map resolution configurable
    constexpr uint32_t shadowMapResolution = 2048;
    if (!m_shadowMap)
    {
        m_shadowMap = GetGpuResourceFactory()->CreateRenderTarget(
            shadowMapResolution,
            shadowMapResolution,
            true);
    }
    if (!m_shadowMap)
    {
        LOG_ERROR(LogShadowPass, "Failed to create a directional shadow map render target.");
        return false;
    }
    if (!m_shadowProgram)
    {
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
        if (!shader->CompileFromSource(vertexSource, fragmentSource, ""))
        {
            LOG_ERROR(LogShadowPass, "Failed to compile the directional shadow map shader.");
            return false;
        }
        m_shadowProgram = shader->GetGpuProgram();
    }
    m_shadowMapReady = true;
    return true;
}

void ShadowPass::Begin_Internal(RenderSystem& rs) const
{
}