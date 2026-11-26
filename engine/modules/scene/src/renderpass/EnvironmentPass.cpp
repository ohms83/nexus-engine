#include "nexus/scene/renderpass/EnvironmentPass.h"
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
}

DEFINE_LOG(EnvironmentPass);

EnvironmentPass::EnvironmentPass()
{
    name = "Environment Pass";
    commandSortType = CommandSortType::FrontToBack;
    pipelineState = {
        .depthTest = true,
        .depthWrite = false,
        .depthFunction = DepthFunction::LesserOrEqual,
        .overrideBlendMode = BlendMode::None,
        .cullMode = PolygonFacing::None,
    };
}

bool EnvironmentPass::Init(const IGpuResourceFactory& factory)
{
    if (!Base::Init(factory)) {
        return false;
    }

    m_skybox = PrimitiveMesh::CreateBox("skybox", glm::vec3(1.0f), factory);
    m_skyboxShader = factory.CreateGpuProgram();
    if (!m_skyboxShader) {
        LOG_ERROR(LogEnvironmentPass, "Can't create skybox's shader!!");
        return false;
    }
    m_skyboxShader->BeginCompile()
        .AddSource(vs, GpuProgram::Type::Vertex)
        .AddSource(fs, GpuProgram::Type::Fragment)
        .Compile();
    return true;
}

void EnvironmentPass::Render(RenderSystem & rs, const ViewContext & viewCtx, const SceneRenderingContext& sceneCtx)
{
    Base::Render(rs, viewCtx, sceneCtx);
    
    auto& scene = *sceneCtx.scene;
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

    m_skyboxShader->Bind();
    m_skyboxShader->SetUniformTextureCube("_Skybox", cubemap, 0);
    m_skyboxShader->SetUniformMatrix("_View", viewPoint->GetViewMatrix(), false);
    m_skyboxShader->SetUniformMatrix("_Projection", viewPoint->GetProjectionMatrix(), false);

    cubemap->Bind();
    vertexBuffer->Bind();
    indexBuffer->Bind();

    // Draw skybox
    auto rhi = rs.GetRenderInterface();
    rs.DrawIndexed(indexBuffer);
}
