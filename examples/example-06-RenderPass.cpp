#include "nexus/Nexus.h"
#include "nexus/graphics/opengl/GLRenderTarget.h"
#include "nexus/graphics/mesh/PlaneMesh.h"

using namespace nxs;

// Vertex and fragment sources for the scene (render the cube into offscreen)
static const char* vs = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 6) in vec2 aTexCoord0;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out vec2 TexCoord0;
void main(){ TexCoord0 = aTexCoord0; gl_Position = projection * view * model * vec4(aPos, 1.0); }
)" ;

static const char* fs = R"(
#version 330 core
in vec2 TexCoord0;
out vec4 color;
uniform sampler2D u_DiffuseMap;
void main(){ color = texture(u_DiffuseMap, TexCoord0); }
)" ;

// Post-process shader to draw the color texture to the default framebuffer
static const char* post_vs = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord0;
out vec2 vUv;
void main(){ vUv = aTexCoord0; gl_Position = vec4(aPos, 1.0); }
)" ;

static const char* post_fs = R"(
#version 330 core
in vec2 vUv;
out vec4 FragColor;
uniform sampler2D u_Texture;
void main(){ FragColor = texture(u_Texture, vUv); }
)" ;

class MiniApp : public nxs::Application
{
public:
    MiniApp() = default;
    ~MiniApp() override = default;

    bool Init_Internal() override
    {
        auto& rs = GetRenderSystem();
        const auto ri = rs.GetRenderInterface();
        rs.SetClearColor(0x303030ff);

        auto& engine = Engine::Instance();

        // Offscreen target
        m_rt = std::make_shared<nxs::GLRenderTarget>(800, 600);
        auto sceneManager = engine.GetSceneManager();
        auto scene = sceneManager->EmplaceAndChange<nxs::Scene>("Editor Scene");
        auto sceneRenderer = std::make_unique<nxs::ForwardSceneRenderer>(GetRenderSystem());
        sceneRenderer->RegisterRenderTarget("offscreen", m_rt);
        scene->SetRenderer(std::move(sceneRenderer));

        // Scene shader
        m_sceneProg.reset(ri->CreateGpuProgram());
        m_sceneProg->BeginCompile()
            .AddSource(vs, nxs::GpuProgram::Type::Vertex)
            .AddSource(fs, nxs::GpuProgram::Type::Fragment)
        .Compile();

        const std::string assetsPath = GetAssetPath("textures/Crate/Wood_Crate_001_basecolor.jpg");
        m_texture = Engine::Instance().GetTextureManager()->Get(assetsPath);
        m_texture->SetFiltering(nxs::TextureFilterMode::Linear, nxs::TextureFilterMode::Linear);

        m_cube = std::make_shared<nxs::CubeMesh>(ri);
        m_transform.SetPosition({0, 0, 0});
        m_camera.width = FLOAT_CAST(GetScreenSize().x);
        m_camera.height = FLOAT_CAST(GetScreenSize().y);
        m_camera.fov = 45.0f;

        // Post-process shader
        m_postProg.reset(ri->CreateGpuProgram());
        m_postProg->BeginCompile()
            .AddSource(post_vs, nxs::GpuProgram::Type::Vertex)
            .AddSource(post_fs, nxs::GpuProgram::Type::Fragment)
        .Compile();

        m_quad = std::make_shared<nxs::PlaneMesh>(ri);
        return true;
    }

    void Render(nxs::RenderSystem& renderSystem) override
    {
        const auto dt = GetDeltaTime();
        m_transform.Rotate(90.0f * dt, glm::vec3(0.0f, 1.0f, 0.0f));
        auto model = m_transform.GetMatrix();
        glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 projection = glm::perspective(glm::radians(m_camera.fov), m_camera.width / m_camera.height, m_camera.nearZ, m_camera.farZ);
        // Render to offscreen target
        m_rt->Bind(renderSystem);
        renderSystem.ClearScreen();
        m_sceneProg->Bind();
        m_sceneProg->SetUniformMatrix("model", model, false);
        m_sceneProg->SetUniformMatrix("view", view, false);
        m_sceneProg->SetUniformMatrix("projection", projection, false);
        m_sceneProg->SetUniformTexture2D("u_DiffuseMap", m_texture->GetProxy(), 0);
        m_cube->GetVertexBuffer()->Bind();
        m_cube->GetIndexBuffer()->Bind();
        renderSystem.DrawIndexed(m_cube->GetIndexBuffer());

        // Blit to default framebuffer from offscreen target
        m_rt->Unbind(renderSystem);
        m_postProg->Bind();
        m_postProg->SetUniformTexture2D("u_Texture", m_rt->GetColorTextureProxy(), 0);
        m_quad->GetVertexBuffer()->Bind();
        m_quad->GetIndexBuffer()->Bind();
        renderSystem.DrawIndexed(m_quad->GetIndexBuffer());
    }

private:
    nxs::Ref<nxs::GpuProgram> m_sceneProg;
    nxs::Ref<nxs::GpuProgram> m_postProg;
    nxs::Ref<nxs::Texture> m_texture;
    nxs::Ref<nxs::Mesh> m_cube;
    nxs::Ref<nxs::Mesh> m_quad;
    nxs::Ref<nxs::GLRenderTarget> m_rt;
    nxs::Transform m_transform;
    nxs::CameraComponent m_camera;
};

int main(int argc, char** argv)
{
    GraphicsConfig config;
    config.screenWidth = 800;
    config.screenHeight = 600;

    Engine::Initialize(nullptr, config);
    auto renderSystem = Engine::Instance().GetRenderSystem();

    // The MiniApp will initialize the render target, shaders, and meshes.

    // Setup the application's main loop: render cube into 'offscreen' then draw the full-screen quad with the color texture.
    // (We use the RunApplication helper to manage the loop like the other examples)

    return nxs::RunApplication<MiniApp>({"RenderPass Example", {nxs::GraphicsAPI::OpenGL, 800, 600, true}, false, true});
}
