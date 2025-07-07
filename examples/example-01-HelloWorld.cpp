#include <iostream>
#include <nexus/Nexus.h>
#include <nexus/graphics/opengl/GLShader.h>

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

class Example_01 final : public nexus::Application
{
public:
    ~Example_01() override
    {
        delete m_vertexBuffer;
        delete m_indexBuffer;
        delete m_shader;
    }

    void Render(nexus::RenderSystem* renderSystem) override
    {
        m_shader->Bind();

        // Calculate matrices (simple orthographic for 2D, or perspective for 3D)
        glm::mat4 model = glm::mat4(1.0f);
        // glm::mat4 view = glm::lookAt(glm::vec3(0, 0, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)); // For 3D
        // glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)pixel_w / (float)pixel_h, 0.1f, 100.0f); // For 3D
        glm::mat4 view = glm::mat4(1.0f); // Identity for 2D
        glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f); // Ortho for 2D

        // Set uniforms
        m_shader->SetUniformMatrix("model", model, false);
        m_shader->SetUniformMatrix("view", view, false);
        m_shader->SetUniformMatrix("projection", projection, false);

        // Bind VAO and draw using EBO
        m_vertexBuffer->Bind();
        m_indexBuffer->Bind();
        glDrawElements(
            GL_TRIANGLES,                     // Draw triangles
            squareIndices.size(),           // Number of indices to draw (6 for our square)
            GL_UNSIGNED_INT,                  // Type of indices
            (void*)0                          // Offset into the EBO (0 means start from the beginning)
        );

        m_vertexBuffer->Unbind();
        m_indexBuffer->Unbind();

        m_shader->Unbind();
    }

protected:
    bool Init_Internal() override
    {
        auto& renderSystem = GetRenderSystem();
        renderSystem.SetClearColor({0.2f, 0.3f, 0.3f, 1.0f});

        constexpr auto vertexSize = sizeof(Vertex);
        const auto bufferSize = squareVertices.size() * vertexSize;
        m_vertexBuffer = new nexus::GLVertexBuffer();
        m_vertexBuffer->Begin()
            .SetVertices(R_CAST<const uint8_t*>(squareVertices.data()), bufferSize)
            .SetUsage(nexus::BufferUsage::StaticDraw)
            .AddAttribute(nexus::VertexAttribute {nexus::VertexAttribute::Type::Position, nexus::DataType::Float, 3})
            .AddAttribute(nexus::VertexAttribute {nexus::VertexAttribute::Type::Color0, nexus::DataType::Float, 3})
            .Build();

        m_indexBuffer = new nexus::GLIndexBuffer();
        m_indexBuffer->Begin()
            .SetIndices(C_CAST<uint32_t*>(squareIndices.data()), squareIndices.size())
            .SetUsage(nexus::BufferUsage::StaticDraw)
            .SetDrawMode(nexus::DrawMode::Triangle)
        .Build();

        m_shader = new nexus::GLShader();
        m_shader->BeginCompile()
            .AddSource(vertexShaderSource, nexus::Shader::Type::Vertex)
            .AddSource(fragmentShaderSource, nexus::Shader::Type::Fragment)
        .Compile();
        return true;
    }

    nexus::VertexBuffer* m_vertexBuffer = nullptr;
    nexus::IndexBuffer* m_indexBuffer = nullptr;
    nexus::Shader* m_shader = nullptr;
};

int main()
{
    nexus::GraphicsConfig graphicsConfig {
        nexus::GraphicsAPI::OpenGL,
        1280, 960,
        false,
    };
    return nexus::RunApplication<Example_01>({
        "Example 1",
        graphicsConfig,
        false,
    });
}
