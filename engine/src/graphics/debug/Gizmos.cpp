//
// Created by nutta on 7/26/2025.
//

#include "graphics/debug/Gizmos.h"
#include "graphics/RenderSystem.h"
#include "memory/BorrowBuffer.h"
#include "scene/Camera.h"

USING_NAMESPACE_NXS;

static Ref<VertexBuffer> s_pointVertexBuffer;
static Ref<IndexBuffer> s_pointIndexBuffer;
static Ref<GpuProgram> s_gpuProgram;

namespace 
{
    struct Vertex
    {
        glm::vec3 pos;
        glm::vec3 color;
    };

    std::vector<Vertex> s_pointVertices;
    std::vector<uint32> s_pointIndices;

    std::vector<Vertex> s_lineVertices;
    std::vector<uint32> s_lineIndices;
}

// Shader sources
static auto s_vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

uniform mat4 _Model;
uniform mat4 _View;
uniform mat4 _Projection;
uniform float _PointSize;

void main()
{
    gl_Position = _Projection * _View * _Model * vec4(aPos, 1.0);
    gl_PointSize = _PointSize;
}
)";

static auto s_fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
uniform vec3 _Color;

void main()
{
    FragColor = vec4(_Color, 1.0);
}
)";

void Gizmos::Init(const RenderSystem& renderSystem)
{
    const auto renderInterface = renderSystem.GetRenderInterface();
    s_pointVertices.emplace_back(glm::vec3 {});
    s_pointIndices.emplace_back(0);

    auto points = std::make_shared<BorrowBuffer>(s_pointVertices);
    
    s_pointVertexBuffer.reset(renderInterface->CreateVertexBuffer());
    s_pointVertexBuffer->Begin()
        .SetVertices(points)
        .SetUsage(BufferUsage::StaticDraw)
        .AddAttribute({VertexAttribute::Type::Position, DataType::Float, 3})
    .Build();

    static uint32_t index = 0;
    s_pointIndexBuffer.reset(renderInterface->CreateIndexBuffer());
    s_pointIndexBuffer->Begin()
        .SetIndices(&index, 1, FrontFace::ClockWise)
        .SetUsage(BufferUsage::StaticDraw)
        .SetDrawMode(DrawMode::Point)
    .Build();

    s_gpuProgram.reset(renderInterface->CreateGpuProgram());
    s_gpuProgram->BeginCompile()
        .AddSource(s_vertexShaderSource, GpuProgram::Type::Vertex)
        .AddSource(s_fragmentShaderSource, GpuProgram::Type::Fragment)
    .Compile();
}

void Gizmos::Cleanup()
{
    s_pointVertexBuffer.reset();
    s_pointIndexBuffer.reset();
    s_gpuProgram.reset();
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
        s_gpuProgram,
        s_pointVertexBuffer,
        s_pointIndexBuffer,
        // Matrices
        {
            {"_Model", translationMtx},
            {"_View", view},
            {"_Projection", projection},
        },
        // Textures
        {},
        // Vec3
        {
            {"_Color", color}
        },
        // Vec4
        {},
        // Float
        {
            {"_PointSize", size}
        },
    };
    command.depthFunction = DepthFunction::Always;
    renderSystem.RegisterDrawCommand(command, RenderPass::Gizmo);
}
