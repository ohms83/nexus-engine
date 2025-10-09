//
// Created by nutta on 6/23/2025.
//
#include "nexus/Nexus.h"
#include <glm/gtc/type_ptr.hpp>

// Shader sources
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 6) in vec2 aTexCoord0;

out vec2 texCoord0;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    texCoord0 = aTexCoord0;
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
in vec2 texCoord0;

uniform sampler2D ourTexture;

void main()
{
    FragColor = texture(ourTexture, texCoord0);
}
)";

static const std::string assetsPath = "assets/textures/Crate/Wood_Crate_001_basecolor.jpg";

class Example_03 final : public nxs::Application
{
public:
    ~Example_03() override = default;

    void Render(nxs::RenderSystem& renderSystem) override
    {
        // Calculate matrices for a rotating cube
        const auto dt = GetDeltaTime();
        m_cubeTransform.Rotate(90.f * dt, glm::vec3(0.5f, 1.0f, 0.0f));

        glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), // Camera position
                                     glm::vec3(0.0f, 0.0f, 0.0f), // Look at origin
                                     glm::vec3(0.0f, 1.0f, 0.0f)  // Up direction
                                    );

        glm::mat4 projection = glm::perspective(glm::radians(m_camera.fov), m_camera.width / m_camera.height, m_camera.nearZ, m_camera.farZ);

        const nxs::RenderCommand renderCommand
        {
            m_gpuProgram,
            m_mesh->GetVertexBuffer(),
            m_mesh->GetIndexBuffer(),
            {
                {"model", m_cubeTransform.GetMatrix()},
                {"view", view},
                {"projection", projection},
            },
            {
                { "ourTexture", 0, m_texture->GetProxy() }
            }
        };

        renderSystem.RegisterDrawCommand(renderCommand);
    }

protected:
    bool Init_Internal() override
    {
        auto& renderSystem = GetRenderSystem();
        const auto renderInterface = renderSystem.GetRenderInterface();
        renderSystem.SetClearColor(0x303030ff);

        m_gpuProgram.reset(renderInterface->CreateGpuProgram());
        m_gpuProgram->BeginCompile()
            .AddSource(vertexShaderSource, nxs::GpuProgram::Type::Vertex)
            .AddSource(fragmentShaderSource, nxs::GpuProgram::Type::Fragment)
        .Compile();

        m_texture = nxs::Engine::Instance().GetTextureManager()->Get(assetsPath);
        m_texture->SetWrapMode(nxs::TextureWrapMode::Clamp, nxs::TextureWrapMode::Clamp);
        m_texture->SetFiltering(nxs::TextureFilterMode::Linear, nxs::TextureFilterMode::Linear);

        m_mesh = std::make_unique<nxs::CubeMesh>(renderInterface);
        return true;
    }

    void OnResize(const glm::ivec2& screenSize, const glm::ivec2& actualSize) override
    {
        Application::OnResize(screenSize, actualSize);
        m_camera.width  = CAST<float>(screenSize.x);
        m_camera.height = CAST<float>(screenSize.y);
    }

    nxs::Ref<nxs::GpuProgram> m_gpuProgram;
    nxs::Ref<nxs::Texture> m_texture;
    nxs::Ptr<nxs::Mesh> m_mesh;
    nxs::Transform m_cubeTransform;
    nxs::CameraProperties m_camera;
};


int main()
{
    constexpr auto vsync = false;
    constexpr auto fullscreen = false;
    nxs::GraphicsConfig graphicsConfig {
        nxs::GraphicsAPI::OpenGL,
        1280, 960,
        vsync,
    };
    return nxs::RunApplication<Example_03>({
        "Example 03",
        graphicsConfig,
        fullscreen
    });
}
