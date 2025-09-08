//
// Created by nutta on 7/26/2025.
//

#include "graphics/debug/Gizmos.h"
#include "graphics/RenderSystem.h"
#include "memory/BorrowBuffer.h"
#include "scene/Camera.h"

#include "Remotery.h"

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
        float size = 1;
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
layout (location = 2) in vec3 aColor;
layout (location = 3) in float aSize;

uniform mat4 _Model;

out vec3 Color;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    gl_PointSize = aSize;
    Color = aColor;
}
)";

static auto s_fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
in vec3 Color;

void main()
{
    FragColor = vec4(Color, 1.0);
}
)";

void Gizmos::Init(const RenderSystem& renderSystem)
{
    s_pointVertices.reserve(50);
    s_pointIndices.reserve(50);

    const auto renderInterface = renderSystem.GetRenderInterface();
    s_pointVertices.emplace_back(glm::vec3 {});
    s_pointIndices.emplace_back(0);

    {
        uint8_t* data = R_CAST<uint8_t*>(s_pointVertices.data());
        uint64_t bufferSize = sizeof(Vertex) * s_pointVertices.capacity();
        auto points = std::make_shared<BorrowBuffer>(data, bufferSize);
        s_pointVertexBuffer.reset(renderInterface->CreateVertexBuffer());
        s_pointVertexBuffer->Begin()
            .SetVertices(points)
            .SetUsage(BufferUsage::DynamicDraw)
            .AddAttribute(VertexAttribute::VertexPosition3D)
            .AddAttribute(VertexAttribute::VertexColor0)
            .AddAttribute({VertexAttribute::Type::Color1, DataType::Float, 1})
        .Build();
    }

    {
        uint8_t* data = R_CAST<uint8_t*>(s_pointIndices.data());
        const uint64_t bufferSize = sizeof(uint32_t) * s_pointIndices.capacity();
        auto indices = std::make_shared<BorrowBuffer>(data, bufferSize);
        s_pointIndexBuffer.reset(renderInterface->CreateIndexBuffer());
        s_pointIndexBuffer->Begin()
            .SetIndices(indices, FrontFace::ClockWise)
            .SetUsage(BufferUsage::DynamicDraw)
            .SetDrawMode(DrawMode::Point)
        .Build();
    }

    s_gpuProgram.reset(renderInterface->CreateGpuProgram());
    s_gpuProgram->BeginCompile()
        .AddSource(s_vertexShaderSource, GpuProgram::Type::Vertex)
        .AddSource(s_fragmentShaderSource, GpuProgram::Type::Fragment)
    .Compile();
}

void Gizmos::Destroy()
{
    s_pointVertexBuffer.reset();
    s_pointIndexBuffer.reset();
    s_gpuProgram.reset();
}

void Gizmos::Clear()
{
    s_pointVertices.clear();
    s_pointIndices.clear();
}

void Gizmos::GenerateDrawCommands(RenderSystem& renderSystem, const glm::mat4& cameraMtx)
{
    rmt_ScopedCPUSample(DrawGizmos, 0);
    // Draw points
    if (s_pointIndices.size() > 0)
    {
        s_pointVertexBuffer->Bind();
        s_pointVertexBuffer->CopyData(s_pointVertices.data(), sizeof(Vertex) * s_pointVertices.size());

        s_pointIndexBuffer->Bind();
        s_pointIndexBuffer->CopyData(s_pointIndices.data(), sizeof(uint32_t) * s_pointIndices.size());
        s_pointIndexBuffer->SetDrawCount(s_pointIndices.size());
        RenderCommand command = {
            s_gpuProgram,
            s_pointVertexBuffer,
            s_pointIndexBuffer,
            // Matrices
            {
                {"_Model", glm::mat4(1)},
            },
        };
        command.depthFunction = DepthFunction::Always;
        renderSystem.RegisterDrawCommand(command, RenderPass::Gizmo);
    }
}

void Gizmos::DrawPoint(
    RenderSystem& renderSystem,
    const glm::vec3& position,
    const Color3F& color,
    float size)
{
    s_pointVertices.emplace_back(Vertex{ position, color, size });
    s_pointIndices.emplace_back(s_pointIndices.size());
}
