//
//  Mesh.cpp
//  Nexus
//
//  Created by nuttachai on 31/10/21.
//

#include "Mesh.hpp"
#include "graphics/RenderSystem.hpp"

USING_NAMESPACE_NXS;

Mesh::Mesh()
{
    
}

void Mesh::init(int primitiveType, int usage, Ref<VertexBuffer> vertexBuffer, const VertexDescription& vertexDesc)
{
    _primitiveType = primitiveType;
    _usage = usage;
    _vertexDesc = vertexDesc;
    
    nexus::VertexBufferCreateInfo bufferInfo =
    {
        {
            // usage
            _usage,
            // Primitives
            _primitiveType,
            // vertexCount
            (uint32_t)_vertices.size(),
            // indexCount
            (uint32_t)_indices.size(),
            // description
            _vertexDesc,
        },
        // vertices
        (float*)_vertices.data(),
        // indices
        _indices.data(),
    };
    
    _vertexBuffer = vertexBuffer;
    _vertexBuffer->init(bufferInfo);
}

const std::vector<float>& Mesh::getVertices() const
{
    return _vertices;
}

std::vector<float>& Mesh::getVertices()
{
    return _vertices;
}

const std::vector<uint32_t>& Mesh::getIndices() const
{
    return _indices;
}
std::vector<uint32_t>& Mesh::getIndices()
{
    return _indices;
}

Ref<Mesh> MeshBuilder::create()
{
    Ref<Mesh> mesh(new Mesh());
    
    uint32_t n = _normals.size() > 0 ? 3 : 0;
    uint32_t t = _texCoords.size()  > 0 ? 2 : 0;
    size_t floatSize = sizeof(float);
    
    VertexDescription vertexDesc = {
        3, 0,
        // Color
        0, 0,
        // Normal
        3 * n, floatSize * 7 * n,
        // Texture Coordinates
        2 * t, (uint32_t)floatSize * 10 * t
    };
    
    // Generate vertex buffer
    auto& vertices = mesh->getVertices();
    for (const auto& vertex : _vertices)
    {
        const glm::vec3& pos = _positions[vertex.positionIndex - 1];
        vertices.insert(vertices.end(), { pos.x, pos.y, pos.z });

        if (vertex.normalIndex > 0)
        {
            const glm::vec3 normal = _normals[vertex.normalIndex - 1];
            vertices.insert(vertices.end(), { normal.x, normal.y, normal.z });
        }

        if (vertex.texCoordIndex > 0)
        {
            const glm::vec2 coord = _texCoords[vertex.texCoordIndex - 1];
            vertices.insert(vertices.end(), { coord.x, coord.y });
        }
    }
    
    mesh->_indices = std::move(_indices);
    return mesh;
}

int Mesh::getPrimitiveFromVertexCount(int count)
{
    int pritmitive = UNDEFINED;
    
    switch (count)
    {
        case 1:
            pritmitive = POINTS;
            break;
        case 2:
            pritmitive = LINES;
            break;
        case 3:
            pritmitive = TRIANGLES;
            break;
        case 4:
            pritmitive = QUADS;
            break;
            
        default:
            // TODO: Error message
            break;
    }
    
    return pritmitive;
}

MeshBuilder& MeshBuilder::addPosition(const glm::vec3& pos)
{
    _positions.emplace_back(pos);
    return *this;
}

MeshBuilder& MeshBuilder::addNormal(const glm::vec3& normal)
{
    _normals.emplace_back(normal);
    return *this;
}

MeshBuilder& MeshBuilder::addTexCoord(const glm::vec2& texCoord)
{
    _texCoords.emplace_back(texCoord);
    return *this;
}

MeshBuilder& MeshBuilder::addPolygon(const std::initializer_list<VertexComponent>& vertices)
{
    return addPolygon(vertices.begin(), vertices.end());
}

MeshBuilder& MeshBuilder::addPolygon(const std::vector<VertexComponent>& vertices)
{
    return addPolygon(vertices.begin(), vertices.end());
}
