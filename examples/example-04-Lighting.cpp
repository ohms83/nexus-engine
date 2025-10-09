//
// Created by nutta on 6/23/2025.
//
#include "nexus/Nexus.h"
#include "imgui.h"
#include <glm/gtc/type_ptr.hpp>

// Shader sources
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 6) in vec2 aTexCoord0;

out vec3 FragPos;  
out vec3 Normal;
out vec2 TexCoord0;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 worldPos = model * vec4(aPos, 1.0);
    gl_Position = projection * view * worldPos;
    FragPos = vec3(worldPos);
    TexCoord0 = aTexCoord0;
    Normal = vec3(model * vec4(aNormal, 1.0));
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord0;

uniform sampler2D u_DiffuseMap;

uniform vec3 u_Ambient;

struct Light {
    // Set w component to 0 for the directional light
    vec3 position;
    // Diffuse color
    vec3 diffuse;
    // Specular color
    vec3 specular;
    float cutoff;

    float constantAtt;
    float linearAtt;
    float quadraticAtt;
};

uniform Light u_DirectLight;
uniform Light u_PointLights[2];

vec3 CalcDirLight(Light light, vec3 normal)
{
    vec3 lightDir = normalize(-light.position);
    float diff = max(dot(normal, lightDir), 0.0);
    return clamp(light.diffuse * diff, 0, 1);
}

vec3 CalcPointLight(Light light, vec3 fragPos, vec3 normal)
{
    vec3 lightDir = light.position - fragPos;
    float dist = length(lightDir);
    if (dist >= light.cutoff) return vec3(0, 0, 0);

    lightDir = normalize(lightDir);
    float diff = max(dot(normal, lightDir), 0.0);
    float attenuation = 1 / (light.constantAtt + (light.linearAtt * diff) + (light.quadraticAtt * diff * diff));
    return clamp(light.diffuse * diff * attenuation, 0, 1);
}

void main()
{
    vec3 N = normalize(Normal);
    vec4 albedo = texture(u_DiffuseMap, TexCoord0);
    vec4 ambient = albedo * vec4(u_Ambient, 1);
    vec4 diffuse = vec4(CalcDirLight(u_DirectLight, N), 1);

    for (int i = 0; i < 2; ++i) {
        diffuse += vec4(CalcPointLight(u_PointLights[i], FragPos, N), 1);
    }
    FragColor = albedo * (ambient + diffuse);
}
)";

class Example_04 final : public nxs::Application
{
public:
    ~Example_04() override = default;

    void Render(nxs::RenderSystem& renderSystem) override
    {
        // Calculate matrices for a rotating cube
        const auto dt = GetDeltaTime();
        m_cubeTransform.Rotate(90.f * dt, glm::vec3(5.0f, 1.0f, 0.0f));
        auto model = m_cubeTransform.GetMatrix();
        glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), // Camera position
                                     glm::vec3(0.0f, 0.0f, 0.0f), // Look at origin
                                     glm::vec3(0.0f, 1.0f, 0.0f)  // Up direction
                                    );

        glm::mat4 projection = glm::perspective(glm::radians(m_camera.fov), m_camera.width / m_camera.height, m_camera.nearZ, m_camera.farZ);
        nxs::RenderCommand renderCommand
        {
            m_gpuProgram,
            m_cubeMesh->GetVertexBuffer(),
            m_cubeMesh->GetIndexBuffer(),
            {
                {"model", model},
                {"view", view},
                {"projection", projection},
            },
            {
                { "u_DiffuseMap", 0, m_texture->GetProxy() },
            },
            {
                { "u_Ambient", m_ambient },
                { "u_DirectLight.position", m_directionalLight.direction },
                { "u_DirectLight.diffuse", m_directionalLight.properties.color },
                { "u_PointLights[0].position", m_lightPositions[0] },
                { "u_PointLights[0].diffuse", m_pointLights[0].properties.color },
                { "u_PointLights[1].position", m_lightPositions[1] },
                { "u_PointLights[1].diffuse", m_pointLights[1].properties.color },
            }
        };
        renderCommand.uniformFloats.emplace_back("u_PointLights[0].cutoff", m_pointLights[0].properties.cutoffRange);
        renderCommand.uniformFloats.emplace_back("u_PointLights[1].cutoff", m_pointLights[1].properties.cutoffRange);

        renderSystem.RegisterDrawCommand(renderCommand);
    }

    void DrawUI() override
    {
        ImGui::Begin("Lighting");
        {
            if (ImGui::TreeNode("Ambient"))
            {
                ImGui::ColorEdit3("Color", R_CAST<float*>(&m_ambient));
                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Directional"))
            {
                ImGui::ColorEdit3("Color", R_CAST<float*>(&m_directionalLight.properties.color));
                ImGui::TreePop();
            }

            ImGui::SeparatorText("Point Lights");
            if (ImGui::TreeNode("Light 0"))
            {
                static bool enableLight = true;
                ImGui::Checkbox("Enable", &enableLight);
                m_pointLights[0].properties.cutoffRange = enableLight ? m_cutoffRange : 0;

                ImGui::ColorEdit3("Color", R_CAST<float*>(&m_pointLights[0].properties.color));
                ImGui::InputFloat3("Position", R_CAST<float*>(&m_lightPositions[0]));
                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Light 1"))
            {
                static bool enableLight = true;
                ImGui::Checkbox("Enable", &enableLight);
                m_pointLights[1].properties.cutoffRange = enableLight ? m_cutoffRange : 0;

                ImGui::ColorEdit3("Color", R_CAST<float*>(&m_pointLights[1].properties.color));
                ImGui::InputFloat3("Position", R_CAST<float*>(&m_lightPositions[1]));
                ImGui::TreePop();
            }
        }
        ImGui::End();
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

        const std::string assetsPath = GetAssetPath("textures/Crate/Wood_Crate_001_basecolor.jpg");
        m_texture = nxs::Engine::Instance().GetTextureManager()->Get(assetsPath);
        m_texture->SetWrapMode(nxs::TextureWrapMode::Clamp, nxs::TextureWrapMode::Clamp);
        m_texture->SetFiltering(nxs::TextureFilterMode::Linear, nxs::TextureFilterMode::Linear);

        m_cubeMesh = std::make_shared<nxs::CubeMesh>(renderInterface);
        m_cubeTransform.SetPosition({0, 0, 0});

        InitLights();
        return true;
    }

    void OnResize(const glm::ivec2& screenSize, const glm::ivec2& actualSize) override
    {
        Application::OnResize(screenSize, actualSize);
        m_camera.width = FLOAT_CAST(screenSize.x);
        m_camera.height = FLOAT_CAST(screenSize.y);
    }

private:
    void InitLights()
    {
        m_ambient = {0.5, 0.5, 0.5};

        m_directionalLight.properties.color = {1, 1, 1};
        m_directionalLight.direction = {10, -10, 0};

        m_pointLights[0].properties.color = {0.5, 0, 0};
        m_lightPositions[0] = {10, 10, 0};

        m_pointLights[1].properties.color = {0, 0.5, 0};
        m_lightPositions[1] = {-10, 10, 0};
    }

protected:
    nxs::Ref<nxs::GpuProgram> m_gpuProgram;
    nxs::Ref<nxs::Texture> m_texture;
    nxs::Ref<nxs::Mesh> m_cubeMesh;
    nxs::Transform m_cubeTransform;
    nxs::CameraProperties m_camera;
    nxs::DirectLightComponent m_directionalLight {};
    nxs::PointLightComponent m_pointLights[2] {};
    glm::vec3 m_ambient {0.5, 0.5, 0.5};
    glm::vec3 m_lightPositions[2] = {};
    float m_cutoffRange = 100;
};


int main()
{
    constexpr auto vsync = true;
    constexpr auto fullscreen = false;
    nxs::GraphicsConfig graphicsConfig {
        nxs::GraphicsAPI::OpenGL,
        1280, 960,
        vsync,
    };
    return nxs::RunApplication<Example_04>({
        "Example 04",
        graphicsConfig,
        fullscreen,
        true
    });
}
