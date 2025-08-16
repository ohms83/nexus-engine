//
// Created by nutta on 6/27/2025.
//

#include <cstring>
#include <nexus/graphics/VertexBuffer.h>

USING_NAMESPACE_NXS;

const VertexAttribute VertexAttribute::VertexPosition3D {
    Type::Position, DataType::Float, 3
};
const VertexAttribute VertexAttribute::VertexPosition2D {
    Type::Position, DataType::Float, 2
};
const VertexAttribute VertexAttribute::VertexNormal {
    Type::Normal, DataType::Float, 3
};
const VertexAttribute VertexAttribute::VertexColor0 {
    Type::Color0, DataType::Float, 3
};
const VertexAttribute VertexAttribute::VertexColor1 {
    Type::Color1, DataType::Float, 3
};
const VertexAttribute VertexAttribute::VertexTangent {
    Type::Tangent, DataType::Float, 3
};
const VertexAttribute VertexAttribute::VertexBinormal {
    Type::Binormal, DataType::Float, 3
};
const VertexAttribute VertexAttribute::VertexTexCoord0 {
    Type::TexCoord0, DataType::Float, 2
};
const VertexAttribute VertexAttribute::VertexTexCoord1 {
    Type::TexCoord1, DataType::Float, 2
};
const VertexAttribute VertexAttribute::VertexTexCoord2 {
    Type::TexCoord2, DataType::Float, 2
};
const VertexAttribute VertexAttribute::VertexTexCoord3 {
    Type::TexCoord3, DataType::Float, 2
};
const VertexAttribute VertexAttribute::VertexTexCoord4 {
    Type::TexCoord4, DataType::Float, 2
};
const VertexAttribute VertexAttribute::VertexTexCoord5 {
    Type::TexCoord5, DataType::Float, 2
};
const VertexAttribute VertexAttribute::VertexTexCoord6 {
    Type::TexCoord6, DataType::Float, 2
};
const VertexAttribute VertexAttribute::VertexTexCoord7 {
    Type::TexCoord7, DataType::Float, 2
};

VertexBuffer& VertexBuffer::Begin()
{
    // Cannot rebuild the buffer.
    assert(m_stride == 0 && !m_hasBuilt);
    m_hasBuilt = true;
    m_handle = Alloc();
    return *this;
}

VertexBuffer& VertexBuffer::SetVertices(const uint8* vertexData, const size_t size)
{
    assert(m_hasBuilt);
    m_vertices.release();
    m_vertices = std::make_unique<uint8[]>(size);
    m_bufferSize = size;
    std::memcpy(m_vertices.get(), vertexData, size);
    return *this;
}

VertexBuffer& VertexBuffer::SetUsage(const BufferUsage usage)
{
    assert(m_hasBuilt);
    m_usage = usage;
    return *this;
}

VertexBuffer& VertexBuffer::AddAttribute(const VertexAttribute& attribute)
{
    assert(m_hasBuilt);
    m_attributes.push_back(attribute);
    m_stride += NxsDataTypeSize(attribute.dataType) * attribute.numElements;
    return *this;
}

void VertexBuffer::Build()
{
    assert(m_hasBuilt);
    m_vertexCount = m_bufferSize / m_stride;
    Build_Impl();
}
