//
// Created by nutta on 7/26/2025.
//

#include "graphics/debug/Gizmos.h"
#include "graphics/RenderSystem.h"
#include "math/Math.h"
#include "memory/BorrowBuffer.h"
#include "scene/Camera.h"

#include "Remotery.h"

USING_NAMESPACE_NXS;

DEFINE_LOG(Gizmos);

namespace 
{
    struct Vertex
    {
        glm::vec3 pos;
        glm::vec3 color;
        float size = 1;
    };

    struct Variables
    {
        std::vector<Vertex> pointVertices;
        std::vector<uint32> pointIndices;
        Ref<VertexBuffer>   pointVertexBuffer;
        Ref<IndexBuffer>    pointIndexBuffer;

        std::vector<Vertex> lineVertices;
        std::vector<uint32> lineIndices;
        Ref<VertexBuffer>   lineVertexBuffer;
        Ref<IndexBuffer>    lineIndexBuffer;

        std::vector<Vertex> boxVertices;
        std::vector<uint32> boxIndices;
        Ref<VertexBuffer>   boxVertexBuffer;
        Ref<IndexBuffer>    boxIndexBuffer;

        std::vector<Vertex> sphereVertices;
        std::vector<uint32> sphereIndices;
        Ref<VertexBuffer>   sphereVertexBuffer;
        Ref<IndexBuffer>    sphereIndexBuffer;

        Ref<GpuProgram>     gpuProgram;

        Variables()
        {
            pointVertices.reserve(100);
            pointIndices.reserve(100);

            lineVertices.reserve(100);
            lineIndices.reserve(100);

            boxVertices.reserve(200);
            boxIndices.reserve(200);

            sphereVertices.reserve(500);
            sphereIndices.reserve(500);
        }

        void ClearData()
        {
            pointVertices.clear();
            pointIndices.clear();
            lineVertices.clear();
            lineIndices.clear();
            boxVertices.clear();
            boxIndices.clear();
            sphereVertices.clear();
            sphereIndices.clear();
        }
    };
    Ptr<Variables> s_va = std::make_unique<Variables>();
}

// Shader sources
static auto s_vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec3 aColor;
layout (location = 3) in float aSize;

uniform mat4 _Model;
// Projection x View matrix
uniform mat4 _CameraMtx;

out vec3 Color;

void main()
{
    gl_Position = _CameraMtx * vec4(aPos, 1.0);
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

static void CreateDrawBuffers(const RenderSystem& renderSystem, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, Ref<VertexBuffer>& vertexBuffer, Ref<IndexBuffer>& indexBuffer, const DrawMode drawMode)
{
    const auto renderInterface = renderSystem.GetRenderInterface();

    {
        uint8_t* data = R_CAST<uint8_t*>(vertices.data());
        uint64_t bufferSize = sizeof(Vertex) * vertices.capacity();
        auto buffer = std::make_shared<BorrowBuffer>(data, bufferSize);
        vertexBuffer.reset(renderInterface->CreateVertexBuffer());
        vertexBuffer->Begin()
            .SetVertices(buffer)
            .SetUsage(BufferUsage::DynamicDraw)
            .AddAttribute(VertexAttribute::VertexPosition3D)
            .AddAttribute(VertexAttribute::VertexColor0)
            .AddAttribute({VertexAttribute::Type::Color1, DataType::Float, 1})
        .Build();
    }

    {
        uint8_t* data = R_CAST<uint8_t*>(indices.data());
        const uint64_t bufferSize = sizeof(uint32_t) * indices.capacity();
        auto indices = std::make_shared<BorrowBuffer>(data, bufferSize);
        indexBuffer.reset(renderInterface->CreateIndexBuffer());
        indexBuffer->Begin()
            .SetIndices(indices, FrontFace::ClockWise)
            .SetUsage(BufferUsage::DynamicDraw)
            .SetDrawMode(drawMode)
        .Build();
    }
}

static void GenerateDrawCommands(
    RenderSystem& renderSystem,
    const glm::mat4& cameraMtx,
    const std::vector<Vertex>& vertices,
    const std::vector<uint32_t>& indices,
    Ref<VertexBuffer> vertexBuffer,
    Ref<IndexBuffer> indexBuffer)
{
    if (indices.size() > 0)
    {
        vertexBuffer->Bind();
        vertexBuffer->CopyData(vertices.data(), sizeof(Vertex) * vertices.size());

        indexBuffer->Bind();
        indexBuffer->CopyData(indices.data(), sizeof(uint32_t) * indices.size());
        indexBuffer->SetDrawCount(indices.size());
        RenderCommand command = {
            s_va->gpuProgram,
            vertexBuffer,
            indexBuffer,
            // Matrices
            {
                {"_Model", glm::mat4(1)},
                {"_CameraMtx", cameraMtx},
            },
        };
        command.depthFunction = DepthFunction::Always;
        renderSystem.RegisterDrawCommand(command, RenderPass::Gizmo);
    }
}

void Gizmos::Init(const RenderSystem& renderSystem)
{
#define CREATE_BUFFER(gizmo, drawMode) CreateDrawBuffers(renderSystem, s_va->##gizmo##Vertices, s_va->##gizmo##Indices, s_va->##gizmo##VertexBuffer, s_va->##gizmo##IndexBuffer, drawMode)
    CREATE_BUFFER(point, DrawMode::Point);
    CREATE_BUFFER(line, DrawMode::Line);
    // TODO: Use DrawMode::LineStrip
    CREATE_BUFFER(box, DrawMode::Line);
    // TODO: Use DrawMode::LineStrip
    CREATE_BUFFER(sphere, DrawMode::Line);
#undef CREATE_BUFFER

    const auto renderInterface = renderSystem.GetRenderInterface();

    s_va->gpuProgram.reset(renderInterface->CreateGpuProgram());
    s_va->gpuProgram->BeginCompile()
        .AddSource(s_vertexShaderSource, GpuProgram::Type::Vertex)
        .AddSource(s_fragmentShaderSource, GpuProgram::Type::Fragment)
    .Compile();
}

void Gizmos::Destroy()
{
    s_va.reset();
}

void Gizmos::Clear()
{
    s_va->ClearData();
}

void Gizmos::ProcessDraw(RenderSystem& renderSystem, const glm::mat4& cameraMtx)
{
    rmt_ScopedCPUSample(DrawGizmos, 0);
#define DRAW_GIZMOS(gizmo) GenerateDrawCommands(renderSystem, cameraMtx, s_va->##gizmo##Vertices, s_va->##gizmo##Indices, s_va->##gizmo##VertexBuffer, s_va->##gizmo##IndexBuffer)
    DRAW_GIZMOS(point);
    DRAW_GIZMOS(line);
    DRAW_GIZMOS(box);
    DRAW_GIZMOS(sphere);
#undef DRAW_GIZMOS
}

void Gizmos::DrawPoint(
    RenderSystem& renderSystem,
    const glm::vec3& position,
    const Color3F& color,
    float size)
{
    s_va->pointVertices.emplace_back(Vertex{ position, color, size });
    s_va->pointIndices.emplace_back(s_va->pointIndices.size());
}

void Gizmos::DrawLine(
    RenderSystem& renderSystem,
    const glm::vec3& start,
    const glm::vec3& end,
    const Color3F& color)
{
    s_va->lineVertices.emplace_back(Vertex{ start, color, 0 });
    s_va->lineVertices.emplace_back(Vertex{ end, color, 0 });
    s_va->lineIndices.emplace_back(s_va->lineIndices.size());
    s_va->lineIndices.emplace_back(s_va->lineIndices.size());
}

void Gizmos::DrawOutlineBox(
    RenderSystem& renderSystem,
    const glm::vec3& position,
    const glm::vec3& extent,
    const glm::quat& rotation,
    const Color3F& color
)
{
    const std::array<glm::vec3, 8> boxVertices = {
        glm::vec3 {-0.5f,  0.5f,  0.5f},
        glm::vec3 {-0.5f, -0.5f,  0.5f},
        glm::vec3 { 0.5f, -0.5f,  0.5f},
        glm::vec3 { 0.5f,  0.5f,  0.5f},

        glm::vec3 {-0.5f,  0.5f, -0.5f},
        glm::vec3 {-0.5f, -0.5f, -0.5f},
        glm::vec3 { 0.5f, -0.5f, -0.5f},
        glm::vec3 { 0.5f,  0.5f, -0.5f},
    };
    const std::array<uint32_t, 24> boxIndices = {
        0, 1,
        1, 2,
        2, 3,
        3, 0,

        4, 5,
        5, 6,
        6, 7,
        7, 4,

        0, 4,
        1, 5,
        2, 6,
        3, 7,
    };

    auto& vertices = s_va->boxVertices;
    auto& indices  = s_va->boxIndices;
    const auto startIndex = vertices.size();

    for (auto& vertex : boxVertices)
    {
        auto pos = (position + (rotation * vertex * extent));
        vertices.emplace_back(pos, color, 0);
    }

    for (const auto index : boxIndices)
    {
        indices.push_back(startIndex + index);
    }
}

void Gizmos::DrawOutlineSphere(
    RenderSystem& renderSystem,
    const glm::vec3& position,
    float radius,
    const glm::quat& rotation,
    uint32_t numSegments,
    const Color3F& xyLatColor,
    const Color3F& yzLatColor,
    const Color3F& longColor)
{
    if (numSegments < 3)
    {
        LOG_WARNING(LogGizmos, std::format("Invalid number of segment. The number of segment must be greather or equal 3. numSegments={}", numSegments));
        return;
    }

    const float thetha = 2 * Math::PI / numSegments;
    auto& vertices = s_va->sphereVertices;
    auto& indices  = s_va->sphereIndices;
    // Create longtitude lines
    {
        const auto startIndex = vertices.size();
        for (int i = 0; i < numSegments; ++i)
        {
            const float rad = thetha * i;
            const float x = radius * cos(rad);
            const float y = 0;
            const float z = radius * sin(rad);
            vertices.emplace_back(position + (rotation * glm::vec3(x, y, z)), longColor, 0);

            const auto i0 = startIndex + i;
            const auto i1 = (i + 1) >= numSegments ? startIndex : i0 + 1;
            indices.push_back(i0);
            indices.push_back(i1);
        }
    }
    // Create x-y latitude lines
    {
        const auto startIndex = vertices.size();
        for (int i = 0; i < numSegments; ++i)
        {
            const float rad = thetha * i;
            const float x = radius * cos(rad);
            const float y = radius * sin(rad);
            const float z = 0;
            vertices.emplace_back(position + (rotation * glm::vec3(x, y, z)), xyLatColor, 0);

            const auto i0 = startIndex + i;
            const auto i1 = (i + 1) >= numSegments ? startIndex : i0 + 1;
            indices.push_back(i0);
            indices.push_back(i1);
        }
    }
    // Create y-z latitude lines
    {
        const auto startIndex = vertices.size();
        for (int i = 0; i < numSegments; ++i)
        {
            const float rad = thetha * i;
            const float x = 0;
            const float y = radius * sin(rad);
            const float z = radius * cos(rad);
            vertices.emplace_back(position + (rotation * glm::vec3(x, y, z)), yzLatColor, 0);

            const auto i0 = startIndex + i;
            const auto i1 = (i + 1) >= numSegments ? startIndex : i0 + 1;
            indices.push_back(i0);
            indices.push_back(i1);
        }
    }
}