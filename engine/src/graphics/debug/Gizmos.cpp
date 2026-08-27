//
// Created by nutta on 7/26/2025.
//

#include "graphics/debug/Gizmos.h"
#include "graphics/RenderSystem.h"
#include "nexus/math/Math.h"
#include "memory/BorrowBuffer.h"

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

    class VertexData
    {
    public:
        VertexData(size_t maxSize, const RenderingInterface& renderInterface, const DrawMode drawMode)
            : m_drawMode(drawMode)
            , m_newMaxVertex(maxSize)
            , m_newMaxIndex(maxSize)
        {
            GenerateBuffers(renderInterface);
        }

        void GenerateBuffers(const RenderingInterface& renderInterface)
        {
            if (m_newMaxVertex > 0)
            {
                LOG_DEBUG(LogGizmos, std::format("Resizing vertex buffer from {} to {}", m_maxVertex, m_newMaxVertex));
                m_vertices.reserve(m_newMaxVertex);
                m_maxVertex = m_newMaxVertex;
                m_newMaxVertex = 0;

                auto* data = R_CAST<uint8_t*>(m_vertices.data());
                uint64_t bufferSize = sizeof(Vertex) * m_vertices.capacity();
                const auto buffer = std::make_shared<BorrowBuffer>(data, bufferSize);
                m_vertexBuffer.reset(renderInterface.CreateVertexBuffer());
                m_vertexBuffer->Begin()
                    .SetVertices(buffer)
                    .SetUsage(BufferUsage::DynamicDraw)
                    .AddAttribute(VertexAttribute::VertexPosition3D)
                    .AddAttribute(VertexAttribute::VertexColor0)
                    .AddAttribute({VertexAttribute::Type::Color1, DataType::Float, 1})
                .Build();
            }

            if (m_newMaxIndex > 0)
            {
                LOG_DEBUG(LogGizmos, std::format("Resizing index buffer from {} to {}", m_maxIndex, m_newMaxIndex));
                m_indices.reserve(m_newMaxIndex);
                m_maxIndex = m_newMaxIndex;
                m_newMaxIndex = 0;

                auto* data = R_CAST<uint8_t*>(m_indices.data());
                const uint64_t bufferSize = sizeof(uint32_t) * m_indices.capacity();
                const auto buffer = std::make_shared<BorrowBuffer>(data, bufferSize);
                m_indexBuffer.reset(renderInterface.CreateIndexBuffer());
                m_indexBuffer->Begin()
                    .SetIndices(buffer, FrontFace::ClockWise)
                    .SetUsage(BufferUsage::DynamicDraw)
                    .SetDrawMode(m_drawMode)
                .Build();
            }
        }

        void AddVertex(const glm::vec3& pos, const glm::vec3& color, float size)
        {
            m_vertices.emplace_back(pos, color, size);

            if (m_vertices.capacity() > m_maxVertex)
            {
                // The vertex buffer will be resized just before it's being used since recreating
                // GPU buffers is expensive and shouldn't be done too frequently
                m_newMaxVertex = m_vertices.capacity();
            }
        }

        void AddIndex(uint32_t index)
        {
            m_indices.push_back(index);

            if (m_indices.capacity() > m_maxIndex)
            {
                // The index buffer will be resized just before it's being used since recreating
                // GPU buffers is expensive and shouldn't be done too frequently
                m_newMaxIndex = m_indices.capacity();
            }
        }

        size_t NumVertex() const
        {
            return m_vertices.size();
        }

        size_t NumIndex() const
        {
            return m_indices.size();
        }

        Ref<VertexBuffer> GetVertexBuffer() const
        {
            return m_vertexBuffer;
        }

        Ref<IndexBuffer> GetIndexBuffer() const
        {
            return m_indexBuffer;
        }
        
        void DrawIndexed(RenderingInterface& renderInterface)
        {
            if (!m_indices.empty())
            {
                if (m_newMaxVertex > 0 || m_newMaxIndex > 0) {
                    GenerateBuffers(renderInterface);
                }

                m_vertexBuffer->Bind();
                m_vertexBuffer->CopyData(m_vertices.data(), sizeof(Vertex) * m_vertices.size(), 0);

                m_indexBuffer->Bind();
                m_indexBuffer->CopyData(m_indices.data(), sizeof(uint32_t) * m_indices.size(), 0);
                m_indexBuffer->SetNumIndexDraw(m_indices.size());

                renderInterface.DrawIndexed(m_indexBuffer);
            }
        }

        //! @brief Copy the data to GPU buffers.
        void CopyToGpu(RenderingInterface& renderInterface)
        {
            if (m_indices.empty()) return;

            if (m_newMaxVertex > 0 || m_newMaxIndex > 0) {
                GenerateBuffers(renderInterface);
            }

            m_vertexBuffer->Bind();
            m_vertexBuffer->CopyData(m_vertices.data(), sizeof(Vertex) * m_vertices.size(), 0);

            m_indexBuffer->Bind();
            m_indexBuffer->CopyData(m_indices.data(), sizeof(uint32_t) * m_indices.size(), 0);
            m_indexBuffer->SetNumIndexDraw(m_indices.size());
        }

        void Clear()
        {
            m_vertices.clear();
            m_indices.clear();
        }

    private:
        DrawMode m_drawMode;
        std::vector<Vertex> m_vertices;
        std::vector<uint32_t> m_indices;
        Ref<VertexBuffer>   m_vertexBuffer;
        Ref<IndexBuffer>    m_indexBuffer;
        size_t m_maxVertex = 0;
        size_t m_maxIndex = 0;
        size_t m_newMaxVertex = 0;
        size_t m_newMaxIndex = 0;
    };

    const int POINT     = 0;
    const int LINE      = 1;
    const int BOX       = 2;
    const int SPHERE    = 3;

    std::map<int, Ref<VertexData>> s_vertexDataList;
    Ref<Shader> s_overlayShader;
    Ref<Material> s_overlayMaterial;
}

// Shader sources
static auto s_vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec3 aColor;
layout (location = 3) in float aSize;

uniform mat4 _Model;
uniform mat4 _View;
uniform mat4 _Projection;

out vec3 Color;

void main()
{
    gl_Position = _Projection * _View * vec4(aPos, 1.0);
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
    const auto renderInterface = renderSystem.GetRenderInterface();

    s_overlayShader = std::make_shared<Shader>("_OverlayShader", 0);
    const auto succeed = s_overlayShader->CompileFromSource(
        *renderInterface,
        s_vertexShaderSource,
        s_fragmentShaderSource,
        "");
    NXS_ASSERT(succeed);

    s_overlayMaterial = std::make_shared<Material>("_OverlayMaterial", 0);
    s_overlayMaterial->SetShader(s_overlayShader);
    s_overlayMaterial->blendMode = BlendMode::None;
    s_overlayMaterial->depthFunction = DepthFunction::Always;
    s_overlayMaterial->depthTest = false;
    s_overlayMaterial->depthWrite = false;

    s_vertexDataList.emplace(POINT, std::make_shared<VertexData>(100, *renderInterface, DrawMode::Point));
    s_vertexDataList.emplace(LINE, std::make_shared<VertexData>(100, *renderInterface, DrawMode::Line));
    s_vertexDataList.emplace(BOX, std::make_shared<VertexData>(200, *renderInterface, DrawMode::Line));
    // TODO: Use DrawMode::LineStrip
    s_vertexDataList.emplace(SPHERE, std::make_shared<VertexData>(200, *renderInterface, DrawMode::Line));
}

void Gizmos::Destroy()
{
    s_overlayShader.reset();
    s_overlayMaterial.reset();
    s_vertexDataList.clear();
}

void Gizmos::Clear()
{
    for (auto [index, vertexData] : s_vertexDataList)
    {
        vertexData->Clear();
    }
}

void Gizmos::ProcessDraw(RenderSystem& renderSystem, const glm::mat4& cameraMtx)
{
    // TODO: Perform drawing during the overlay render pass
#if 0
    rmt_ScopedCPUSample(DrawGizmos, 0);
    auto& renderInterface = *renderSystem.GetRenderInterface();
    renderInterface.SetDepthFunction(DepthFunction::Always);

    s_gpuProgram->Bind();
    s_gpuProgram->SetUniformMatrix("_Model", glm::mat4(1), false);
    s_gpuProgram->SetUniformMatrix("_CameraMtx", cameraMtx, false);

    for (auto [index, vertexData] : s_vertexDataList)
    {
        vertexData->DrawIndexed(renderInterface);
    }
#endif
}

void Gizmos::CreateRenderCommands(std::vector<RenderCommand>& outCommands, RenderSystem& renderSystem)
{
    static const Sphere boundingSphere { glm::vec3(0), FLT_MAX };

    auto renderInterface = renderSystem.GetRenderInterface();
    for (auto [index, vertexData] : s_vertexDataList)
    {
        if (vertexData->NumVertex() == 0 || vertexData->NumIndex() == 0) continue;

        vertexData->CopyToGpu(*renderInterface);

        auto vertexBuffer = vertexData->GetVertexBuffer();
        auto indexBuffer = vertexData->GetIndexBuffer();
        NXS_ASSERT(vertexBuffer && indexBuffer);

        RenderCommand command {};
        command.vertexBuffer = vertexBuffer;
        command.indexBuffer = indexBuffer;
        command.indexCount = indexBuffer->GetNumIndexDraw();
        command.material = s_overlayMaterial;
        command.bounds = boundingSphere;

        // Override pipeline states
        command.pipelineOverrides.depthFunction = DepthFunction::Always;
        command.pipelineOverrides.depthWrite = false;
        command.pipelineOverrides.depthTest = false;
        // vertexData->DrawIndexed(renderInterface);

        outCommands.push_back(command);
    }
}

void Gizmos::DrawPoint(
    RenderSystem& renderSystem,
    const glm::vec3& position,
    const Color3F& color,
    float size)
{
    auto pointVertex = s_vertexDataList[POINT];
    pointVertex->AddVertex(position, color, size);
    pointVertex->AddIndex(pointVertex->NumIndex());
}

void Gizmos::DrawLine(
    RenderSystem& renderSystem,
    const glm::vec3& start,
    const glm::vec3& end,
    const Color3F& color)
{
    auto lineVertex = s_vertexDataList[LINE];
    lineVertex->AddVertex(start, color, 0);
    lineVertex->AddVertex(end, color, 0 );
    lineVertex->AddIndex(lineVertex->NumIndex());
    lineVertex->AddIndex(lineVertex->NumIndex());
}

void Gizmos::DrawLocalAxes(
    RenderSystem& renderSystem,
    const glm::vec3& position,
    const glm::quat& rotation,
    float axisLength,
    const std::array<Color3F, 3>& axisColors)
{
    std::array<glm::vec3, 3> axes = {
        glm::vec3 {axisLength, 0, 0},
        glm::vec3 {0, axisLength, 0},
        glm::vec3 {0, 0, axisLength},
    };

    for (int i = 0; i < 3; ++i)
    {
        DrawLine(renderSystem, position, position + (rotation * axes[i]), axisColors[i]);
    }
}

void Gizmos::DrawOutlineBox(
    RenderSystem& renderSystem,
    const glm::vec3& center,
    const glm::vec3& extent,
    const glm::mat4& transform,
    const Color3F& color)
{
    const std::array<glm::vec3, 8> boxVertices = {
        glm::vec3 {-1.0f,  1.0f,  1.0f},
        glm::vec3 {-1.0f, -1.0f,  1.0f},
        glm::vec3 { 1.0f, -1.0f,  1.0f},
        glm::vec3 { 1.0f,  1.0f,  1.0f},

        glm::vec3 {-1.0f,  1.0f, -1.0f},
        glm::vec3 {-1.0f, -1.0f, -1.0f},
        glm::vec3 { 1.0f, -1.0f, -1.0f},
        glm::vec3 { 1.0f,  1.0f, -1.0f},
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

    auto boxVertex = s_vertexDataList[BOX];
    const auto startIndex = boxVertex->NumVertex();

    for (auto& vertex : boxVertices)
    {
        auto pos = transform * glm::vec4(center + (vertex * extent), 1);
        boxVertex->AddVertex(pos, color, 0);
    }

    for (const auto index : boxIndices)
    {
        boxVertex->AddIndex(startIndex + index);
    }
}

void Gizmos::DrawOutlineSphere(
    RenderSystem& renderSystem,
    const glm::vec3& position,
    const float radius,
    const glm::mat4& transform,
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

    auto sphereVertex = s_vertexDataList[SPHERE];
    const float thetha = 2 * Math::PI / FLOAT_CAST(numSegments);
    // Create longtitude lines
    {
        const auto startIndex = sphereVertex->NumVertex();
        for (int i = 0; i < numSegments; ++i)
        {
            const float rad = thetha * FLOAT_CAST(i);
            const float x = radius * cos(rad);
            const float y = 0;
            const float z = radius * sin(rad);
            const auto pos = transform * glm::vec4(position + glm::vec3(x, y, z), 1);
            sphereVertex->AddVertex(pos, longColor, 0);

            const auto i0 = startIndex + i;
            const auto i1 = (i + 1) >= numSegments ? startIndex : i0 + 1;
            sphereVertex->AddIndex(i0);
            sphereVertex->AddIndex(i1);
        }
    }
    // Create x-y latitude lines
    {
        const auto startIndex = sphereVertex->NumVertex();
        for (int i = 0; i < numSegments; ++i)
        {
            const float rad = thetha * FLOAT_CAST(i);
            const float x = radius * cos(rad);
            const float y = radius * sin(rad);
            const float z = 0;
            const auto pos = transform * glm::vec4(position + glm::vec3(x, y, z), 1);
            sphereVertex->AddVertex(pos, xyLatColor, 0);

            const auto i0 = startIndex + i;
            const auto i1 = (i + 1) >= numSegments ? startIndex : i0 + 1;
            sphereVertex->AddIndex(i0);
            sphereVertex->AddIndex(i1);
        }
    }
    // Create y-z latitude lines
    {
        const auto startIndex = sphereVertex->NumVertex();
        for (int i = 0; i < numSegments; ++i)
        {
            const float rad = thetha * FLOAT_CAST(i);
            const float x = 0;
            const float y = radius * sin(rad);
            const float z = radius * cos(rad);
            const auto pos = transform * glm::vec4(position + glm::vec3(x, y, z), 1);
            sphereVertex->AddVertex(pos, yzLatColor, 0);

            const auto i0 = startIndex + i;
            const auto i1 = (i + 1) >= numSegments ? startIndex : i0 + 1;
            sphereVertex->AddIndex(i0);
            sphereVertex->AddIndex(i1);
        }
    }
}