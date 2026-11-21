#include "nexus/scene/renderer/SkyboxRenderer.h"
#include "nexus/graphics/GpuResourceFactory.h"
#include "nexus/graphics/RenderSystem.h"
#include "nexus/graphics/CubeTexture.h"
#include "nexus/scene/Scene.h"
#include "nexus/scene/SceneView.h"

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
            gl_Position = vec4(_Projection * _View * vec4(aPos, 1.0)).xyww;
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

SkyboxRenderer::SkyboxRenderer(const IGpuResourceFactory& gpuFactory)
{
    m_skybox = PrimitiveMesh::CreateBox("skybox", glm::vec3(1.0f), gpuFactory);
    m_shader = gpuFactory.CreateGpuProgram();
    m_shader->AddSource(vs, GpuProgram::Type::Vertex);
    m_shader->AddSource(fs, GpuProgram::Type::Fragment);
    m_shader->Compile();
}

void SkyboxRenderer::Render(RenderSystem &rs, const Scene &scene, const ISceneView& viewPoint)
{
    auto cubemap = scene.Skybox();
    if (!cubemap) return;

    auto vertexBuffer = m_skybox->GetVertexBuffer();
    auto indexBuffer = m_skybox->GetIndexBuffer();

    m_shader->Bind();
    m_shader->SetUniformTexture2D("_Skybox", cubemap, 0);
    m_shader->SetUniformMatrix("_View", viewPoint.GetViewMatrix());
    m_shader->SetUniformMatrix("_Projection", viewPoint.GetProjectionMatrix());

    cubemap->Bind();
    vertexBuffer->Bind();
    indexBuffer->Bind();
    rs.DrawIndexed(indexBuffer);
}