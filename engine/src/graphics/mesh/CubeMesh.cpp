#include "graphics/Mesh.h"
#include "graphics/RenderingInterface.h"
#include "nexus/memory/OwningBuffer.h"

USING_NAMESPACE_NXS;

namespace
{
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoord0;
    };

    // Vertices for a standard cube (24 vertices for 6 faces * 4 vertices/face)
    std::vector<Vertex> s_vertices = {
        // Front face (Z+)
        // Position             Normal                 TexCoord
        {{-0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {0.0f, 0.0f}}, // 0
        {{ 0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 0.0f}}, // 1
        {{ 0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 1.0f}}, // 2
        {{-0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {0.0f, 1.0f}}, // 3
        // Back face (Z-)
        {{-0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {1.0f, 0.0f}}, // 4
        {{ 0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {0.0f, 0.0f}}, // 5
        {{ 0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {0.0f, 1.0f}}, // 6
        {{-0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {1.0f, 1.0f}}, // 7
        // Top face (Y+)
        {{-0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, {0.0f, 1.0f}}, // 8 (same pos as 3)
        {{ 0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 1.0f}}, // 9 (same pos as 2)
        {{ 0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 0.0f}}, // 10 (same pos as 6)
        {{-0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, {0.0f, 0.0f}}, // 11 (same pos as 7)
        // Bottom face (Y-)
        {{-0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, {0.0f, 0.0f}}, // 12 (same pos as 0)
        {{ 0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, {1.0f, 0.0f}}, // 13 (same pos as 1)
        {{ 0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, {1.0f, 1.0f}}, // 14 (same pos as 5)
        {{-0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, {0.0f, 1.0f}}, // 15 (same pos as 4)
        // Right face (X+)
        {{ 0.5f, -0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}}, // 16 (same pos as 1)
        {{ 0.5f, -0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}}, // 17 (same pos as 5)
        {{ 0.5f,  0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}}, // 18 (same pos as 6)
        {{ 0.5f,  0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}}, // 19 (same pos as 2)
        // Left face (X-)
        {{-0.5f, -0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}}, // 20 (same pos as 0)
        {{-0.5f, -0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}}, // 21 (same pos as 4)
        {{-0.5f,  0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}}, // 22 (same pos as 7)
        {{-0.5f,  0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}}
    };

    std::vector<uint32_t> s_indices = {
        // Front face (Z+): CCW when viewed from +Z
        0, 1, 2,
        0, 2, 3,

        // Back face (Z-): CCW when viewed from -Z
        6, 5, 4,
        6, 4, 7,

        // Top face (Y+): CCW when viewed from +Y
        8, 9, 10,
        8, 10, 11,

        // Bottom face (Y-): CCW when viewed from -Y
        12, 14, 13,
        12, 15, 14,

        // Right face (X+): CCW when viewed from +X
        16, 17, 18,
        16, 18, 19,

        // Left face (X-): CCW when viewed from -X
        21, 20, 23,
        21, 23, 22
    };
}

Ref<Mesh> PrimitiveMesh::CreateBox(
    std::string name,
    const glm::vec3& size,
    const RenderingInterface& renderingInterface,
    Ref<Material> material)
{
    // Scale the unit cube vertices by the specified size
    std::vector<Vertex>* scaledVertices = new std::vector<Vertex>();
    scaledVertices->reserve(s_vertices.size());
    for (const auto& v : s_vertices)
    {
        Vertex sv;
        sv.position = v.position * size;
        sv.normal = v.normal;
        sv.texCoord0 = v.texCoord0;
        scaledVertices->push_back(sv);
    }

    Ref<IBuffer> vertexData = std::make_shared<OwningBuffer>(
        (uint8_t*)scaledVertices->data(), sizeof(Vertex) * scaledVertices->size());

    Ref<VertexBuffer> vertexBuffer;
    vertexBuffer.reset(renderingInterface.CreateVertexBuffer());
    vertexBuffer->Begin()
        .SetVertices(vertexData)
        .SetUsage(BufferUsage::StaticDraw)
        .AddAttribute(VertexAttribute {VertexAttribute::Type::Position, DataType::Float, 3})
        .AddAttribute(VertexAttribute {VertexAttribute::Type::Normal, DataType::Float, 3})
        .AddAttribute(VertexAttribute {VertexAttribute::Type::TexCoord0, DataType::Float, 2})
    .Build();

    std::vector<uint32_t>* indices = new std::vector<uint32_t>(s_indices);

    Ref<IBuffer> indexData = std::make_shared<OwningBuffer>(
        (uint8_t*)indices->data(), sizeof(uint32_t) * indices->size());
    Ref<IndexBuffer> indexBuffer;
    indexBuffer.reset(renderingInterface.CreateIndexBuffer());
    indexBuffer->Begin()
        .SetIndices(indexData, FrontFace::CounterClockWise)
        .SetUsage(BufferUsage::StaticDraw)
        .SetDrawMode(DrawMode::Triangle)
    .Build();

    Ref<Mesh> mesh = std::make_shared<Mesh>(name);
    mesh->SetVertexBuffer(vertexBuffer);
    mesh->SetIndexBuffer(indexBuffer);
    if (material)
    {
        mesh->SetMaterial(material);
    }
    mesh->ComputeBounds();
    return mesh;
}