//
// Created by nutta on 7/22/2025.
//

#include "graphics/mesh/PlaneMesh.h"
#include "graphics/RenderingInterface.h"

USING_NAMESPACE_NXS;

namespace
{
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoord;
    };

    std::vector<Vertex> vertices = {
        // Position          Normal             Texcoord0
        {{-0.5f, 0.0f, -0.5f}, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f }},
        {{-0.5f, 0.0f,  0.5f}, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f }},
        {{ 0.5f, 0.0f,  0.5f}, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f }},
        {{ 0.5f, 0.0f, -0.5f}, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f }},
    };

    std::vector<uint32> indices = {
        0, 1, 2,
        0, 2, 3,
    };
}

PlaneMesh::PlaneMesh()
    : Mesh("PlaneMesh")
{
    const auto& renderInterface = RenderingInterface::Instance();

    constexpr auto vertexSize = sizeof(Vertex);
    const auto bufferSize = vertices.size() * vertexSize;
    m_vertexBuffer.reset(renderInterface.CreateVertexBuffer());
    m_vertexBuffer->Begin()
        .SetVertices(R_CAST<const uint8_t*>(vertices.data()), bufferSize)
        .SetUsage(BufferUsage::StaticDraw)
        .AddAttribute(VertexAttribute {VertexAttribute::Type::Position, DataType::Float, 3})
        .AddAttribute(VertexAttribute {VertexAttribute::Type::Normal, DataType::Float, 3})
        .AddAttribute(VertexAttribute {VertexAttribute::Type::TexCoord0, DataType::Float, 2})
    .Build();

    m_indexBuffer.reset(renderInterface.CreateIndexBuffer());
    m_indexBuffer->Begin()
        .SetIndices(indices.data(), indices.size(), FrontFace::CounterClockWise)
        .SetUsage(BufferUsage::StaticDraw)
        .SetDrawMode(DrawMode::Triangle)
    .Build();
}
