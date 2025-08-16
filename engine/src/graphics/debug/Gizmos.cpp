//
// Created by nutta on 7/26/2025.
//

#include "nexus/graphics/debug/Gizmos.h"
#include "nexus/graphics/RenderSystem.h"
#include "nexus/scene/Camera.h"

USING_NAMESPACE_NXS;

static Ref<VertexBuffer> pointVertex;
static Ref<IndexBuffer> pointIndex;
static Ref<Shader> shader;

// Shader sources
static auto vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform float u_PointSize;

void main()
{
    gl_Position = u_Projection * u_View * u_Model * vec4(aPos, 1.0);
    gl_PointSize = u_PointSize;
}
)";

static auto fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
// in vec3 outColor;
uniform vec3 u_Color;

void main()
{
    FragColor = vec4(u_Color, 1.0);
}
)";

void Gizmos::Init(const RenderSystem& renderSystem)
{
    const auto renderInterface = renderSystem.GetRenderInterface();
    static glm::vec3 point{};
    pointVertex.reset(renderInterface->CreateVertexBuffer());
    pointVertex->Begin()
        .SetVertices(R_CAST<const uint8_t*>(&point), sizeof(point))
        .SetUsage(BufferUsage::StaticDraw)
        .AddAttribute({VertexAttribute::Type::Position, DataType::Float, 3})
    .Build();

    static uint32_t index = 0;
    pointIndex.reset(renderInterface->CreateIndexBuffer());
    pointIndex->Begin()
        .SetIndices(&index, 1, FrontFace::ClockWise)
        .SetUsage(BufferUsage::StaticDraw)
        .SetDrawMode(DrawMode::Point)
    .Build();

    shader.reset(renderInterface->CreateShader());
    shader->BeginCompile()
        .AddSource(vertexShaderSource, Shader::Type::Vertex)
        .AddSource(fragmentShaderSource, Shader::Type::Fragment)
    .Compile();
}

void Gizmos::Cleanup()
{
    pointVertex.reset();
    pointIndex.reset();
    shader.reset();
}

void Gizmos::DrawPoint(
    RenderSystem& renderSystem,
    const glm::vec3& position,
    const Color3F& color,
    const glm::mat4& view,
    const glm::mat4& projection,
    float size)
{
    glm::mat4 translationMtx = glm::translate(glm::mat4(1), position);
    RenderCommand command = {
        shader,
        pointVertex,
        pointIndex,
        // Matrices
        {
            {"u_Model", translationMtx},
            {"u_View", view},
            {"u_Projection", projection},
        },
        // Textures
        {},
        // Vec3
        {
            {"u_Color", color}
        },
        // Vec4
        {},
        // Float
        {
            {"u_PointSize", size}
        },
    };
    command.depthFunction = DepthFunction::Always;
    renderSystem.RegisterDrawCommand(command, RenderPass::Gizmo);
}
