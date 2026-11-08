//
// Created by nutta on 7/22/2025.
//

#include "nexus/graphics/Mesh.h"
#include "nexus/container/Array.h"
#include "nexus/graphics/RenderingInterface.h"
#include "nexus/memory/OwningBuffer.h"

USING_NAMESPACE_NXS;

namespace
{
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoord;
    };

    Array<Vertex> s_vertices = {
        // Position          Normal             Texcoord0
        {{-0.5f, 0.0f, -0.5f}, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f }},
        {{-0.5f, 0.0f,  0.5f}, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f }},
        {{ 0.5f, 0.0f,  0.5f}, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f }},
        {{ 0.5f, 0.0f, -0.5f}, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f }},
    };

    Array<uint32_t> s_indices = {
        0, 1, 2,
        0, 2, 3,
    };
}

Ref<Mesh> PrimitiveMesh::CreatePlane(
    std::string name,
    float width, float height,
    const RenderingInterface& renderingInterface,
    Ref<Material> material)
{
    Array<Vertex>* vertices = new Array<Vertex> {
        // Position                             Normal                Texcoord0
        {{-0.5f * width, 0.0f, -0.5f * height}, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f }},
        {{-0.5f * width, 0.0f,  0.5f * height}, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f }},
        {{ 0.5f * width, 0.0f,  0.5f * height}, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f }},
        {{ 0.5f * width, 0.0f, -0.5f * height}, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f }},
    };
    Ref<IBuffer> vertexData = std::make_shared<OwningBuffer>(
        (uint8_t*)vertices->data(), sizeof(Vertex) * vertices->size());

    Ref<VertexBuffer> vertexBuffer;
    vertexBuffer.reset(renderingInterface.CreateVertexBuffer());
    vertexBuffer->Begin()
        .SetVertices(vertexData)
        .SetUsage(BufferUsage::StaticDraw)
        .AddAttribute(VertexAttribute {VertexAttribute::Type::Position, DataType::Float, 3})
        .AddAttribute(VertexAttribute {VertexAttribute::Type::Normal, DataType::Float, 3})
        .AddAttribute(VertexAttribute {VertexAttribute::Type::TexCoord0, DataType::Float, 2})
    .Build();

    Array<uint32_t>* indices = new Array<uint32_t> {
        0, 1, 2,
        0, 2, 3,
    };

    Ref<IBuffer> indexData = std::make_shared<OwningBuffer>(
        (uint8_t*)indices->data(), sizeof(uint32_t) * indices->size());
    Ref<IndexBuffer> indexBuffer;
    indexBuffer.reset(renderingInterface.CreateIndexBuffer());
    indexBuffer->Begin()
        .SetIndices(indexData, FrontFace::CounterClockWise)
        .SetUsage(BufferUsage::StaticDraw)
        .SetDrawMode(DrawMode::Triangle)
    .Build();

    auto mesh = std::make_shared<Mesh>(std::move(name));
    mesh->SetVertexBuffer(vertexBuffer);
    mesh->SetIndexBuffer(indexBuffer);
    mesh->SetMaterial(material);
    mesh->ComputeBounds();
    return mesh;
}