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
    glm::vec2 texCoord0;
};

// Vertices for a standard cube (24 vertices for 6 faces * 4 vertices/face)
const std::vector<Vertex> cubeVertices = {
    // Front face (Z+)
    // Position             TexCoord
    {{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f}}, // 0
    {{ 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f}}, // 1
    {{ 0.5f,  0.5f,  0.5f}, {1.0f, 1.0f}}, // 2
    {{-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f}}, // 3
    // Back face (Z-)
    {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f}}, // 4
    {{ 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}}, // 5
    {{ 0.5f,  0.5f, -0.5f}, {0.0f, 1.0f}}, // 6
    {{-0.5f,  0.5f, -0.5f}, {1.0f, 1.0f}}, // 7
    // Top face (Y+)
    {{-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f}}, // 8 (same pos as 3)
    {{ 0.5f,  0.5f,  0.5f}, {1.0f, 1.0f}}, // 9 (same pos as 2)
    {{ 0.5f,  0.5f, -0.5f}, {1.0f, 0.0f}}, // 10 (same pos as 6)
    {{-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f}}, // 11 (same pos as 7)
    // Bottom face (Y-)
    {{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f}}, // 12 (same pos as 0)
    {{ 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f}}, // 13 (same pos as 1)
    {{ 0.5f, -0.5f, -0.5f}, {1.0f, 1.0f}}, // 14 (same pos as 5)
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}}, // 15 (same pos as 4)
    // Right face (X+)
    {{ 0.5f, -0.5f,  0.5f}, {0.0f, 0.0f}}, // 16 (same pos as 1)
    {{ 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f}}, // 17 (same pos as 5)
    {{ 0.5f,  0.5f, -0.5f}, {1.0f, 1.0f}}, // 18 (same pos as 6)
    {{ 0.5f,  0.5f,  0.5f}, {0.0f, 1.0f}}, // 19 (same pos as 2)
    // Left face (X-)
    {{-0.5f, -0.5f,  0.5f}, {1.0f, 0.0f}}, // 20 (same pos as 0)
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}}, // 21 (same pos as 4)
    {{-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f}}, // 22 (same pos as 7)
    {{-0.5f,  0.5f,  0.5f}, {1.0f, 1.0f}}
};

static const std::vector<uint32_t> cubeIndices  = {
    // Front face
    0, 1, 2,  // First triangle
    2, 3, 0,  // Second triangle

    // Back face
    4, 5, 6,
    6, 7, 4,

    // Top face
    8, 9, 10,
    10, 11, 8,

    // Bottom face
    12, 13, 14,
    14, 15, 12,

    // Right face
    16, 17, 18,
    18, 19, 16,

    // Left face
    20, 21, 22,
    22, 23, 20
};

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

class Example_03 final : public nexus::Application
{
public:
    ~Example_03() override
    {
        delete m_vertexBuffer;
        delete m_indexBuffer;
        delete m_shader;
        delete m_textureProxy;
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
            m_shader,
            m_vertexBuffer,
            m_indexBuffer,
            {
                {"model", m_cubeTransform.GetMatrix()},
                {"view", view},
                {"projection", projection},
            },
            {
                { "ourTexture", 0, m_textureProxy }
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
        m_vertexBuffer = renderInterface.CreateVertexBuffer();
        m_vertexBuffer->Begin()
            .SetVertices(R_CAST<const uint8_t*>(cubeVertices.data()), bufferSize)
            .SetUsage(nexus::BufferUsage::StaticDraw)
            .AddAttribute(nexus::VertexAttribute {nexus::VertexAttribute::Type::Position, nexus::DataType::Float, 3})
            .AddAttribute(nexus::VertexAttribute {nexus::VertexAttribute::Type::TexCoord0, nexus::DataType::Float, 2})
        .Build();
        std::cout << "Vertex stride = " << vertexSize << std::endl;

        m_indexBuffer = renderInterface.CreateIndexBuffer();
        m_indexBuffer->Begin()
            .SetIndices(C_CAST<uint32_t*>(cubeIndices .data()), cubeIndices .size())
            .SetUsage(nexus::BufferUsage::StaticDraw)
            .SetDrawMode(nexus::DrawMode::Triangle)
        .Build();

        m_shader = renderInterface.CreateShader();
        m_shader->BeginCompile()
            .AddSource(vertexShaderSource, nexus::Shader::Type::Vertex)
            .AddSource(fragmentShaderSource, nexus::Shader::Type::Fragment)
        .Compile();

        m_texture = nexus::TextureManager::GetInstance().Get(assetsPath);
        m_texture->SetWrapMode(nexus::TextureWrapMode::Clamp, nexus::TextureWrapMode::Clamp);
        m_texture->SetFiltering(nexus::TextureFilterMode::Linear, nexus::TextureFilterMode::Linear);
        m_textureProxy = m_texture->AllocateGpuResource(renderInterface);
        return true;
    }

    nexus::VertexBuffer* m_vertexBuffer = nullptr;
    nexus::IndexBuffer* m_indexBuffer = nullptr;
    nexus::Shader* m_shader = nullptr;
    nexus::TextureProxy* m_textureProxy = nullptr;
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
