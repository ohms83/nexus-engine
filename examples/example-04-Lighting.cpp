//
// Created by nutta on 6/23/2025.
//
#include <nexus/Nexus.h>

#include "nexus/scene/Transform.h"
#include <glm/gtc/type_ptr.hpp>

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
};
uniform Light u_Light;

vec3 CalcDirLight(Light light, vec3 normal)
{
    vec3 lightDir = normalize(light.position);
    float diff = max(dot(normal, lightDir), 0.0);
    return light.diffuse * diff;
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

class Example_03 final : public nexus::Application
{
public:
    ~Example_03() override
    {
    }
    void Render(nexus::RenderSystem& renderSystem) override
    {
        const auto screenSize = GetWindowSize();

        // Calculate matrices for a rotating cube
        const auto dt = GetDeltaTime();
        m_cubeTransform.Rotate(90.f * dt, glm::vec3(0.5f, 1.0f, 0.0f));

        glm::mat4 view = glm::lookAt(glm::vec3(0, 0, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)); // Camera position
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)screenSize.x / (float)screenSize.y, 0.1f, 100.0f);

        const nexus::RenderCommand renderCommand
        {
            m_shader.get(),
            m_vertexBuffer.get(),
            m_indexBuffer.get(),
            {
                {"model", m_cubeTransform.GetMatrix()},
                {"view", view},
                {"projection", projection},
            },
            {
                { "ourTexture", 0, m_textureProxy.get() },
            },
            {
                { "u_Ambient", glm::vec3(0.2, 0.2, 0.2) },
                { "u_Light.position", glm::vec3(1, 1, 0) },
                { "u_Light.diffuse", glm::vec3(1, 1, 1) },
            }
        };

        renderSystem.RegisterDrawCommand(renderCommand);
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
            .SetUsage(nexus::BufferUsage::StaticDraw)
            .AddAttribute(nexus::VertexAttribute {nexus::VertexAttribute::Type::Position, nexus::DataType::Float, 3})
            .AddAttribute(nexus::VertexAttribute {nexus::VertexAttribute::Type::Normal, nexus::DataType::Float, 3})
            .AddAttribute(nexus::VertexAttribute {nexus::VertexAttribute::Type::TexCoord0, nexus::DataType::Float, 2})
        .Build();
        std::cout << "Vertex stride = " << vertexSize << std::endl;

        m_indexBuffer.reset(renderInterface.CreateIndexBuffer());
        m_indexBuffer->Begin()
            .SetIndices(C_CAST<uint32_t*>(cubeIndices .data()), cubeIndices .size())
            .SetUsage(nexus::BufferUsage::StaticDraw)
            .SetDrawMode(nexus::DrawMode::Triangle)
        .Build();

        m_shader.reset(renderInterface.CreateShader());
        m_shader->BeginCompile()
            .AddSource(vertexShaderSource, nexus::Shader::Type::Vertex)
            .AddSource(fragmentShaderSource, nexus::Shader::Type::Fragment)
        .Compile();

        m_texture = nexus::TextureManager::GetInstance().Get(assetsPath);
        m_texture->SetWrapMode(nexus::TextureWrapMode::Clamp, nexus::TextureWrapMode::Clamp);
        m_texture->SetFiltering(nexus::TextureFilterMode::Linear, nexus::TextureFilterMode::Linear);
        m_textureProxy.reset(m_texture->AllocateGpuResource(renderInterface));
        return true;
    }

    nexus::Ptr<nexus::VertexBuffer> m_vertexBuffer;
    nexus::Ptr<nexus::IndexBuffer> m_indexBuffer;
    nexus::Ptr<nexus::Shader> m_shader;
    nexus::Ptr<nexus::TextureProxy> m_textureProxy;
    nexus::Ref<nexus::Texture> m_texture;
    nexus::Transform m_cubeTransform;
};


int main()
{
    constexpr auto vsync = true;
    constexpr auto fullscreen = false;
    nexus::GraphicsConfig graphicsConfig {
        nexus::GraphicsAPI::OpenGL,
        1280, 960,
        vsync,
    };
    return nexus::RunApplication<Example_03>({
        "Example 03",
        graphicsConfig,
        fullscreen
    });
}
