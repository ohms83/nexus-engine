#include <iostream>
#include <nexus/Nexus.h>

// Define Vertex structure
struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
};

// Vertices for a square
const std::vector<Vertex> squareVertices = {
    {{-0.5f,  0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}}, // Top-left       - Index 0
    {{ 0.5f,  0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}}, // Top-right      - Index 1
    {{ 0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}}, // Bottom-right   - Index 2
    {{-0.5f, -0.5f, 0.0f}, {1.0f, 1.0f, 0.0f}}  // Bottom-left    - Index 3
};

// Indices for the square (two triangles)
const std::vector<unsigned int> squareIndices = {
    0, 1, 2, // First triangle
    2, 3, 0  // Second triangle
};

// Shader sources
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec3 aColor;

out vec3 ourColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    ourColor = aColor;
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
in vec3 ourColor;

void main()
{
    FragColor = vec4(ourColor, 1.0);
}
)";

class Example_01 final : public nxs::Application
{
public:
    void Render(nxs::RenderSystem& renderSystem) override
    {
        // Calculate matrices (simple orthographic for 2D, or perspective for 3D)
        glm::mat4 model = glm::mat4(1.0f);
        // glm::mat4 view = glm::lookAt(glm::vec3(0, 0, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)); // For 3D
        // glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)pixel_w / (float)pixel_h, 0.1f, 100.0f); // For 3D
        glm::mat4 view = glm::mat4(1.0f); // Identity for 2D
        glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f); // Ortho for 2D

        const nxs::RenderCommand renderCommand
        {
            m_shader,
            m_vertexBuffer,
            m_indexBuffer,
            {
                {"model", model},
                {"view", view},
                {"projection", projection},
            }
        };

        renderSystem.RegisterDrawCommand(renderCommand);
    }

protected:
    bool Init_Internal() override
    {
        auto& renderSystem = GetRenderSystem();
        const auto& renderInterface = renderSystem.GetRenderInterface();
        renderSystem.SetClearColor({0.2f, 0.3f, 0.3f, 1.0f});

        constexpr auto vertexSize = sizeof(Vertex);
        const auto bufferSize = squareVertices.size() * vertexSize;
        m_vertexBuffer.reset(renderInterface.CreateVertexBuffer());
        m_vertexBuffer->Begin()
            .SetVertices(R_CAST<const uint8_t*>(squareVertices.data()), bufferSize)
            .SetUsage(nxs::BufferUsage::StaticDraw)
            .AddAttribute(nxs::VertexAttribute {nxs::VertexAttribute::Type::Position, nxs::DataType::Float, 3})
            .AddAttribute(nxs::VertexAttribute {nxs::VertexAttribute::Type::Color0, nxs::DataType::Float, 3})
        .Build();

        m_indexBuffer.reset(renderInterface.CreateIndexBuffer());
        m_indexBuffer->Begin()
            .SetIndices(C_CAST<uint32_t*>(squareIndices.data()), squareIndices.size(), nxs::FrontFace::ClockWise)
            .SetUsage(nxs::BufferUsage::StaticDraw)
            .SetDrawMode(nxs::DrawMode::Triangle)
        .Build();

        m_shader.reset(renderInterface.CreateShader());
        m_shader->BeginCompile()
            .AddSource(vertexShaderSource, nxs::Shader::Type::Vertex)
            .AddSource(fragmentShaderSource, nxs::Shader::Type::Fragment)
        .Compile();
        return true;
    }

    nxs::Ref<nxs::VertexBuffer> m_vertexBuffer;
    nxs::Ref<nxs::IndexBuffer> m_indexBuffer;
    nxs::Ref<nxs::Shader> m_shader;
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
    return nxs::RunApplication<Example_01>({
        "Example 1",
        graphicsConfig,
        fullscreen,
    });
}
