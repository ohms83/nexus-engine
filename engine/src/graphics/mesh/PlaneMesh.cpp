//
// Created by nutta on 7/22/2025.
//

#include "graphics/mesh/PlaneMesh.h"
#include "graphics/RenderingInterface.h"
#include "memory/BorrowBuffer.h"
#include "memory/OwningBuffer.h"

USING_NAMESPACE_NXS;

namespace
{
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoord;
    };

    std::vector<Vertex> s_vertices = {
        // Position          Normal             Texcoord0
        {{-0.5f, 0.0f, -0.5f}, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f }},
        {{-0.5f, 0.0f,  0.5f}, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f }},
        {{ 0.5f, 0.0f,  0.5f}, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f }},
        {{ 0.5f, 0.0f, -0.5f}, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f }},
    };

    std::vector<uint32_t> s_indices = {
        0, 1, 2,
        0, 2, 3,
    };
}

PlaneMesh::PlaneMesh(const Ref<RenderingInterface>& renderingInterface)
    : Mesh("PlaneMesh")
{
    static uint64 count = 0;
    m_name = std::format("PlaneMesh_{}", count++);

    // constexpr auto vertexSize = sizeof(Vertex);
    // const auto bufferSize = vertices.size() * vertexSize;
    Ref<IBuffer> vertexData = std::make_shared<BorrowBuffer>(s_vertices);

    m_vertexBuffer.reset(renderingInterface->CreateVertexBuffer());
    m_vertexBuffer->Begin()
        .SetVertices(vertexData)
        .SetUsage(BufferUsage::StaticDraw)
        .AddAttribute(VertexAttribute {VertexAttribute::Type::Position, DataType::Float, 3})
        .AddAttribute(VertexAttribute {VertexAttribute::Type::Normal, DataType::Float, 3})
        .AddAttribute(VertexAttribute {VertexAttribute::Type::TexCoord0, DataType::Float, 2})
    .Build();

    Ref<IBuffer> indexData = std::make_shared<BorrowBuffer>(s_indices);
    m_indexBuffer.reset(renderingInterface->CreateIndexBuffer());
    m_indexBuffer->Begin()
        .SetIndices(indexData, FrontFace::CounterClockWise)
        .SetUsage(BufferUsage::StaticDraw)
        .SetDrawMode(DrawMode::Triangle)
    .Build();
}

Ref<Mesh> PrimitiveMesh::CreatePlane(std::string name, Ref<RenderingInterface> renderingInterface, float width, float height)
{
    std::vector<Vertex>* vertices = new std::vector<Vertex> {
        // Position                             Normal                Texcoord0
        {{-0.5f * width, 0.0f, -0.5f * height}, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f }},
        {{-0.5f * width, 0.0f,  0.5f * height}, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f }},
        {{ 0.5f * width, 0.0f,  0.5f * height}, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f }},
        {{ 0.5f * width, 0.0f, -0.5f * height}, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f }},
    };
    Ref<IBuffer> vertexData = std::make_shared<OwningBuffer>(
        (uint8_t*)vertices->data(), sizeof(Vertex) * vertices->size());

    Ref<VertexBuffer> vertexBuffer;
    vertexBuffer.reset(renderingInterface->CreateVertexBuffer());
    vertexBuffer->Begin()
        .SetVertices(vertexData)
        .SetUsage(BufferUsage::StaticDraw)
        .AddAttribute(VertexAttribute {VertexAttribute::Type::Position, DataType::Float, 3})
        .AddAttribute(VertexAttribute {VertexAttribute::Type::Normal, DataType::Float, 3})
        .AddAttribute(VertexAttribute {VertexAttribute::Type::TexCoord0, DataType::Float, 2})
    .Build();

    std::vector<uint32_t>* indices = new std::vector<uint32_t> {
        0, 1, 2,
        0, 2, 3,
    };

    Ref<IBuffer> indexData = std::make_shared<OwningBuffer>(
        (uint8_t*)indices->data(), sizeof(uint32_t) * indices->size());
    Ref<IndexBuffer> indexBuffer;
    indexBuffer.reset(renderingInterface->CreateIndexBuffer());
    indexBuffer->Begin()
        .SetIndices(indexData, FrontFace::CounterClockWise)
        .SetUsage(BufferUsage::StaticDraw)
        .SetDrawMode(DrawMode::Triangle)
    .Build();

    auto mesh = std::make_shared<Mesh>(std::move(name));
    mesh->SetVertexBuffer(vertexBuffer);
    mesh->SetIndexBuffer(indexBuffer);
    return mesh;
}