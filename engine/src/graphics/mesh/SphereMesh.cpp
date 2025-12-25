#include "graphics/Mesh.h"
#include "graphics/RenderingInterface.h"
#include "memory/OwningBuffer.h"
#include "core/LogDispatcher.h"

USING_NAMESPACE_NXS;

namespace
{
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoord0;
    };
}

Ref<Mesh> PrimitiveMesh::CreateSphere(
    std::string name,
    float radius,
    uint32_t sectorCount,
    uint32_t stackCount,
    Ref<RenderingInterface> renderingInterface,
    Ref<Material> material)
{
    NXS_ASSERT(sectorCount);
    NXS_ASSERT(stackCount);

    auto vertices = new Vertex[ (sectorCount + 1) * (stackCount + 1) ];
    auto indices = new uint32_t[ sectorCount * stackCount * 6 ];
    uint32_t vertexIndex = 0;
    uint32_t indexIndex = 0;

    for (uint32_t i = 0; i <= stackCount; ++i)
    {
        float stackAngle = glm::pi<float>() / 2 - i * glm::pi<float>() / stackCount; // from pi/2 to -pi/2
        float xy = radius * cosf(stackAngle);             // r * cos(u)
        float z = radius * sinf(stackAngle);              // r * sin(u)

        for (uint32_t j = 0; j <= sectorCount; ++j)
        {
            float sectorAngle = j * 2 * glm::pi<float>() / sectorCount; // from 0 to 2pi

            Vertex vertex;
            vertex.position.x = xy * cosf(sectorAngle); // r * cos(u) * cos(v)
            vertex.position.y = xy * sinf(sectorAngle); // r * cos(u) * sin(v)
            vertex.position.z = z;                       // r * sin(u)

            glm::vec3 norm = glm::normalize(vertex.position);
            vertex.normal = norm;

            vertex.texCoord0.x = (float)j / sectorCount;
            vertex.texCoord0.y = (float)i / stackCount;

            vertices[vertexIndex++] = vertex;
        }
    }

    for (uint32_t i = 0; i < stackCount; ++i)
    {
        uint32_t k1 = i * (sectorCount + 1);     // beginning of current stack
        uint32_t k2 = k1 + sectorCount + 1;      // beginning of next stack

        for (uint32_t j = 0; j < sectorCount; ++j, ++k1, ++k2)
        {
            if (i != 0)
            {
                indices[indexIndex++] = k1;
                indices[indexIndex++] = k2;
                indices[indexIndex++] = k1 + 1;
            }

            if (i != (stackCount - 1))
            {
                indices[indexIndex++] = k1 + 1;
                indices[indexIndex++] = k2;
                indices[indexIndex++] = k2 + 1;
            }
        }
    }

    Ref<IBuffer> vertexData = std::make_shared<OwningBuffer>(
        (uint8_t*)vertices, sizeof(Vertex) * (sectorCount + 1) * (stackCount + 1));
    Ref<VertexBuffer> vertexBuffer;
    vertexBuffer.reset(renderingInterface->CreateVertexBuffer());
    vertexBuffer->Begin()
        .SetVertices(vertexData)
        .SetUsage(BufferUsage::StaticDraw)
        .AddAttribute(VertexAttribute {VertexAttribute::Type::Position, DataType::Float, 3})
        .AddAttribute(VertexAttribute {VertexAttribute::Type::Normal, DataType::Float, 3})
        .AddAttribute(VertexAttribute {VertexAttribute::Type::TexCoord0, DataType::Float, 2})
    .Build();

    Ref<IBuffer> indexData = std::make_shared<OwningBuffer>(
        (uint8_t*)indices, sizeof(uint32_t) * sectorCount * stackCount * 6);
    Ref<IndexBuffer> indexBuffer;
    indexBuffer.reset(renderingInterface->CreateIndexBuffer());
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