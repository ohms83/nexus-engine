#include "nexus/Nexus.h"

using namespace nxs;

int main(int argc, char** argv)
{
    GraphicsConfig config;
    config.screenWidth = 800;
    config.screenHeight = 600;

    Engine::Initialize(nullptr, config);
    auto& renderSystem = Engine::Instance().m_renderSystem;

    // Register a simple depth-only pass
    RenderPass depthPass;
    depthPass.SetName("DepthPrepass");
    depthPass.SetPriority(RENDER_PASS_DEPTH_FILL);
    depthPass.pipelineState.depthTest = true;
    depthPass.pipelineState.depthWrite = true;
    depthPass.SetGlobalShader(nullptr);
    renderSystem->GetRenderInterface();

    // TODO: Construct a scene and register passes

    Engine::Destroy();
    return 0;
}
