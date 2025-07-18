//
// Created by nutta on 6/23/2025.
//
#include <nexus/Nexus.h>

#include "nexus/scene/Transform.h"
#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "nexus/resource/Material.h"
#include "nexus/resource/Texture.h"

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord0;
};

// Vertices for a standard cube (24 vertices for 6 faces * 4 vertices/face)
const std::vector<Vertex> cubeVertices = {
    // Front face (Z+)
    // Position             Normal                 TexCoord
    {{-0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {0.0f, 0.0f}}, // 0
    {{ 0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 0.0f}}, // 1
    {{ 0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 1.0f}}, // 2
    {{-0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {0.0f, 1.0f}}, // 3
    // Back face (Z-)
    {{-0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {1.0f, 0.0f}}, // 4
    {{ 0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {0.0f, 0.0f}}, // 5
    {{ 0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {0.0f, 1.0f}}, // 6
    {{-0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {1.0f, 1.0f}}, // 7
    // Top face (Y+)
    {{-0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, {0.0f, 1.0f}}, // 8 (same pos as 3)
    {{ 0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 1.0f}}, // 9 (same pos as 2)
    {{ 0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 0.0f}}, // 10 (same pos as 6)
    {{-0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, {0.0f, 0.0f}}, // 11 (same pos as 7)
    // Bottom face (Y-)
    {{-0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, {0.0f, 0.0f}}, // 12 (same pos as 0)
    {{ 0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, {1.0f, 0.0f}}, // 13 (same pos as 1)
    {{ 0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, {1.0f, 1.0f}}, // 14 (same pos as 5)
    {{-0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, {0.0f, 1.0f}}, // 15 (same pos as 4)
    // Right face (X+)
    {{ 0.5f, -0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}}, // 16 (same pos as 1)
    {{ 0.5f, -0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}}, // 17 (same pos as 5)
    {{ 0.5f,  0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}}, // 18 (same pos as 6)
    {{ 0.5f,  0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}}, // 19 (same pos as 2)
    // Left face (X-)
    {{-0.5f, -0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}}, // 20 (same pos as 0)
    {{-0.5f, -0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}}, // 21 (same pos as 4)
    {{-0.5f,  0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}}, // 22 (same pos as 7)
    {{-0.5f,  0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}}
};

static const std::vector<uint32_t> cubeIndices = {
    // Front face (Z+): Vertices 0, 1, 2, 3. View from +Z. (BL, BR, TR, TL)
    0, 3, 2,  // Triangle 1: Bottom-Left, Top-Left, Top-Right (CCW)
    0, 2, 1,  // Triangle 2: Bottom-Left, Top-Right, Bottom-Right (CCW)

    // Back face (Z-): Vertices 4, 5, 6, 7. View from -Z.
    // Relative order when viewed from -Z: 6 (BL), 7 (BR), 4 (TR), 5 (TL)
    6, 4, 5,  // Triangle 1: Bottom-Left, Top-Left, Top-Right (CCW)
    6, 7, 4,  // Triangle 2: Bottom-Left, Bottom-Right, Top-Right (CCW)

    // Top face (Y+): Vertices 8, 9, 10, 11. View from +Y.
    // Relative order when viewed from +Y: 8 (BL), 9 (BR), 10 (TR), 11 (TL)
    8, 11, 10, // Triangle 1: Bottom-Left, Top-Left, Top-Right (CCW)
    8, 10, 9,  // Triangle 2: Bottom-Left, Top-Right, Bottom-Right (CCW)

    // Bottom face (Y-): Vertices 12, 13, 14, 15. View from -Y.
    // Relative order when viewed from -Y: 15 (BL), 14 (BR), 13 (TR), 12 (TL)
    15, 12, 13, // Triangle 1: Bottom-Left, Top-Left, Top-Right (CCW)
    15, 13, 14, // Triangle 2: Bottom-Left, Top-Right, Bottom-Right (CCW)

    // Right face (X+): Vertices 16, 17, 18, 19. View from +X.
    // Relative order when viewed from +X: 16 (BL), 17 (BR), 18 (TR), 19 (TL)
    16, 19, 18, // Triangle 1: Bottom-Left, Top-Left, Top-Right (CCW)
    16, 18, 17, // Triangle 2: Bottom-Left, Top-Right, Bottom-Right (CCW)

    // Left face (X-): Vertices 20, 21, 22, 23. View from -X.
    // Relative order when viewed from -X: 21 (BL), 20 (BR), 23 (TR), 22 (TL)
    21, 22, 23, // Triangle 1: Bottom-Left, Top-Left, Top-Right (CCW)
    21, 23, 20  // Triangle 2: Bottom-Left, Top-Right, Bottom-Right (CCW)
};

// Shader sources
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 6) in vec2 aTexCoord0;

out vec3 FragPos;  
out vec3 Normal;
out vec2 texCoord0;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 worldPos = model * vec4(aPos, 1.0);
    gl_Position = projection * view * worldPos;
    FragPos = vec3(worldPos);
    texCoord0 = aTexCoord0;
    Normal = vec3(model * vec4(aNormal, 1.0));
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 texCoord0;

uniform sampler2D ourTexture;

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
uniform Light u_Light;
uniform Light u_PointLights[2];

vec3 CalcDirLight(Light light, vec3 normal)
{
    vec3 lightDir = normalize(light.position);
    float diff = max(dot(normal, lightDir), 0.0);
    return light.diffuse * diff;
}

vec3 CalcPointLight(Light light, vec3 fragPos, vec3 normal)
{
    vec3 lightDir = light.position - fragPos;
    float dist = length(lightDir);
    if (dist >= light.cutoff) return vec3(0);

    lightDir = normalize(lightDir);
    float diff = max(dot(normal, lightDir), 0.0);
    float attenuation = 1 / (light.constantAtt + (light.linearAtt * diff) + (light.quadraticAtt * diff * diff));
    return light.diffuse * diff * attenuation;
}

void main()
{
    vec4 albedo = texture(ourTexture, texCoord0);
    vec4 ambient = albedo * vec4(u_Ambient, 1);
    vec4 diffuse = albedo * vec4(CalcDirLight(u_Light, Normal), 1);
    FragColor = ambient + diffuse;
}
)";

static const std::string assetsPath = "assets/textures/Crate/Wood_Crate_001_basecolor.jpg";

class Example_04 final : public nxs::Application
{
public:
    ~Example_04() override
    {
    }
    void Render(nxs::RenderSystem& renderSystem) override
    {
        // Calculate matrices for a rotating cube
        const auto dt = GetDeltaTime();
        m_cubeTransform.Rotate(90.f * dt, glm::vec3(0.5f, 1.0f, 0.0f));

        const nxs::RenderCommand renderCommand
        {
            m_shader.get(),
            m_vertexBuffer.get(),
            m_indexBuffer.get(),
            {
                    {"model", m_cubeTransform.GetMatrix()},
                    {"view", m_camera.GetViewMtx()},
                    {"projection", m_camera.GetProjectionMtx()},
            },
            {
                { "ourTexture", 0, m_texture->GetProxy() },
            },
            {
                { "u_Ambient", m_ambient },
                { "u_Light.position", glm::vec3(1, 1, 0) },
                { "u_Light.diffuse", m_directionalLight.diffuseColor    },
            }
        };

        renderSystem.RegisterDrawCommand(renderCommand);
    }

    void RenderEditor(const nxs::RenderSystem& renderSystem) override
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
                ImGui::ColorEdit3("Color", R_CAST<float*>(&m_directionalLight.diffuseColor));
                ImGui::TreePop();
            }

            ImGui::SeparatorText("Point Lights");
            if (ImGui::TreeNode("Light 0"))
            {
                static bool enableLight = false;
                static float position[] = {0, 0, 0};
                ImGui::Checkbox("Enable", &enableLight);
                ImGui::ColorEdit3("Color", R_CAST<float*>(&m_pointLights[0].diffuseColor));
                ImGui::InputFloat3("Position", position);
                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Light 1"))
            {
                static bool enableLight = false;
                ImGui::Checkbox("Enable", &enableLight);
                ImGui::ColorEdit3("Color", R_CAST<float*>(&m_pointLights[1].diffuseColor));
                ImGui::TreePop();
            }
        }
        ImGui::End();
    }

protected:
    bool Init_Internal() override
    {
        auto& renderSystem = GetRenderSystem();
        const auto& renderInterface = renderSystem.GetRenderInterface();
        renderSystem.SetClearColor(0x303030ff);

        constexpr auto vertexSize = sizeof(Vertex);
        const auto bufferSize = cubeVertices.size() * vertexSize;
        m_vertexBuffer.reset(renderInterface.CreateVertexBuffer());
        m_vertexBuffer->Begin()
            .SetVertices(R_CAST<const uint8_t*>(cubeVertices.data()), bufferSize)
            .SetUsage(nxs::BufferUsage::StaticDraw)
            .AddAttribute(nxs::VertexAttribute {nxs::VertexAttribute::Type::Position, nxs::DataType::Float, 3})
            .AddAttribute(nxs::VertexAttribute {nxs::VertexAttribute::Type::Normal, nxs::DataType::Float, 3})
            .AddAttribute(nxs::VertexAttribute {nxs::VertexAttribute::Type::TexCoord0, nxs::DataType::Float, 2})
        .Build();
        std::cout << "Vertex stride = " << vertexSize << std::endl;

        m_indexBuffer.reset(renderInterface.CreateIndexBuffer());
        m_indexBuffer->Begin()
            .SetIndices(C_CAST<uint32_t*>(cubeIndices .data()), cubeIndices .size())
            .SetUsage(nxs::BufferUsage::StaticDraw)
            .SetDrawMode(nxs::DrawMode::Triangle)
        .Build();

        m_shader.reset(renderInterface.CreateShader());
        m_shader->BeginCompile()
            .AddSource(vertexShaderSource, nxs::Shader::Type::Vertex)
            .AddSource(fragmentShaderSource, nxs::Shader::Type::Fragment)
        .Compile();

        m_texture = nxs::TextureManager::GetInstance().Get(assetsPath);
        m_texture->SetWrapMode(nxs::TextureWrapMode::Clamp, nxs::TextureWrapMode::Clamp);
        m_texture->SetFiltering(nxs::TextureFilterMode::Linear, nxs::TextureFilterMode::Linear);
        m_texture->AllocateGpuResource(renderInterface);

        InitLights();
        return true;
    }

    void OnResize(const glm::ivec2& screenSize, const glm::ivec2& actualSize) override
    {
        Application::OnResize(screenSize, actualSize);
        m_camera.transform.LookAt({0, 0, 3}, {0, 0, 0}, {0, 1, 0});
        m_camera.SetProjection(45.f, CAST<float>(actualSize.x), CAST<float>(actualSize.y), 0.1f, 100.f);
    }

private:
    void InitLights()
    {
        m_ambient = {0.5, 0.5, 0.5};

        m_directionalLight.diffuseColor = {1, 1, 1};
        m_directionalLight.transform.SetPosition({10, 10, 0});

        m_pointLights[0].diffuseColor = {0.5, 0, 0};
        m_pointLights[0].transform.SetPosition({10, 10, 0});

        m_pointLights[1].diffuseColor = {0, 0.5, 0};
        m_pointLights[1].transform.SetPosition({-10, 10, 0});
    }

protected:
    nxs::Ptr<nxs::VertexBuffer> m_vertexBuffer;
    nxs::Ptr<nxs::IndexBuffer> m_indexBuffer;
    nxs::Ptr<nxs::Shader> m_shader;
    nxs::Ref<nxs::Texture> m_texture;
    nxs::Transform m_cubeTransform;
    nxs::Camera m_camera;
    nxs::DirectionalLight m_directionalLight {};
    nxs::PointLight m_pointLights[2] {};
    glm::vec3 m_ambient {0.5, 0.5, 0.5};
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
        "Example 03",
        graphicsConfig,
        fullscreen,
        true
    });
}
