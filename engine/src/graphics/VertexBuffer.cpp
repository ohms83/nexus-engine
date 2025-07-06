//
// Created by nutta on 6/27/2025.
//

#include <nexus/graphics/VertexBuffer.h>

USING_NAMESPACE_NXS;

VertexBuffer& VertexBuffer::Begin(Buffer&& vertexData, BufferUsage usage)
{
    // Cannot rebuild the buffer.
    assert(m_stride == 0 && !m_hasBuilt);
    m_hasBuilt = true;
    m_usage = usage;
    m_vertexData = std::move(vertexData);
    return *this;
}

VertexBuffer& VertexBuffer::Begin(uint8* data, size_t size, BufferUsage usage)
{
    // Cannot rebuild the buffer.
    assert(m_stride == 0 && !m_hasBuilt);
    m_hasBuilt = true;
    m_usage = usage;
    m_vertexData.Take(data, size);
    return *this;
}

VertexBuffer& VertexBuffer::AddAttribute(const VertexAttribute& attribute)
{
    assert(m_hasBuilt);
    m_attributes.push_back(attribute);
    m_stride += NxsDataTypeSize(attribute.dataType);
    return *this;
}

void VertexBuffer::Build()
{
    assert(m_hasBuilt);
    m_vertexCount = m_vertexData.Size() / m_stride;
    Build_Impl();
}
