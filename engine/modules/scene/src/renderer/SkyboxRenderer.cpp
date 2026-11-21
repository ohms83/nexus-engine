#include "nexus/scene/renderer/SkyboxRenderer.h"
#include "nexus/graphics/GpuResourceFactory.h"
#include "nexus/graphics/RenderSystem.h"
#include "nexus/graphics/CubeTexture.h"
#include "nexus/scene/Scene.h"
#include "nexus/scene/SceneView.h"
#include "nexus/scene/component/SceneNodeComponent.h"
#include "nexus/scene/component/CameraComponent.h"

USING_NAMESPACE_NXS;

namespace
{
    static const std::string vs = R"(
        #version 450
        layout (location = 0) in vec3 aPos;
        invariant gl_Position;
        out vec3 TexCoord0;
        uniform mat4 _View;
        uniform mat4 _Projection;
        void main()
        {
            TexCoord0 = aPos;
            mat4 skyboxView = mat4(mat3(_View));
            gl_Position = vec4(_Projection * skyboxView * vec4(aPos, 1.0)).xyww;
        }
    )";

    static const std::string fs = R"(
        #version 450
        out vec4 FragColor;
        in vec3 TexCoord0;
        uniform samplerCube _Skybox;
        void main()
        {
            FragColor = texture(_Skybox, TexCoord0);
        }
    )";

    PipelineState pipelineState = {
        .depthTest = true,
        .depthWrite = false,
        .depthFunction = DepthFunction::LesserOrEqual,
        .overrideBlendMode = BlendMode::None,
        .cullMode = PolygonFacing::None,
    };
}

SkyboxRenderer::SkyboxRenderer(const IGpuResourceFactory& gpuFactory)
{
    m_skybox = PrimitiveMesh::CreateBox("skybox", glm::vec3(1.0f), gpuFactory);
    m_shader = gpuFactory.CreateGpuProgram();
    m_shader->BeginCompile()
        .AddSource(vs, GpuProgram::Type::Vertex)
        .AddSource(fs, GpuProgram::Type::Fragment)
        .Compile();
}

void SkyboxRenderer::Render(RenderSystem &rs, const Scene &scene)
{
    auto cubemap = scene.Skybox();
    if (!cubemap) return;

    const ISceneView* viewPoint = nullptr;
    const auto& registry = *scene.GetRegistry();
    for (const auto view = registry.view<const SceneNodeComponent, const CameraComponent>();
         const auto& [entity, nodeComp, cameraComp] : view.each())
    {
        viewPoint = D_CAST<const ISceneView*>(SceneNode::FindNode(nodeComp.id));
        if (viewPoint) break;
    }
    if (!viewPoint) return;

    auto vertexBuffer = m_skybox->GetVertexBuffer();
    auto indexBuffer = m_skybox->GetIndexBuffer();

    m_shader->Bind();
    m_shader->SetUniformTextureCube("_Skybox", cubemap, 0);
    m_shader->SetUniformMatrix("_View", viewPoint->GetViewMatrix(), false);
    m_shader->SetUniformMatrix("_Projection", viewPoint->GetProjectionMatrix(), false);

    cubemap->Bind();
    vertexBuffer->Bind();
    indexBuffer->Bind();

    // Draw skybox
    auto rhi = rs.GetRenderInterface();
    rhi->ApplyPipelineState(pipelineState);
    rs.DrawIndexed(indexBuffer);
}