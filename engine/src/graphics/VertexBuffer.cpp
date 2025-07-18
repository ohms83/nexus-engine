//
// Created by nutta on 6/27/2025.
//

#include <cstring>
#include <nexus/graphics/VertexBuffer.h>

USING_NAMESPACE_NXS;

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
