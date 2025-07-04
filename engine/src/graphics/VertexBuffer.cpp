//
// Created by nutta on 6/27/2025.
//

#include <nexus/graphics/VertexBuffer.h>

USING_NAMESPACE_NXS;

static std::array<size_t, SIZE_CAST(DataType::Num)> s_dataSizes = {
    sizeof(uint8),
    sizeof(uint16),
    sizeof(uint32),
    sizeof(uint64),
    sizeof(int8),
    sizeof(int16),
    sizeof(int32),
    sizeof(int64),
    sizeof(float),
    sizeof(double),
};

VertexBuffer::VertexBuffer()
{
    for (size_t i = 0; i < m_attribOffsets.size(); i++)
    {
        m_attribOffsets[i] = -1;
    }
}

VertexBuffer& VertexBuffer::Begin()
{
    // Cannot rebuild the buffer.
    assert(m_stride == 0 && !m_hasBuilt);
    m_hasBuilt = true;
    return *this;
}

VertexBuffer& VertexBuffer::AddAttribute(const VertexAttribute& attribute)
{
    assert(m_hasBuilt);

    const int attribIndex = INT_CAST(attribute.type);
    const int dataTypeIndex = INT_CAST(attribute.dataType);
    // Cannot add the same attribute type again.
    assert(m_attribOffsets[attribIndex] == -1);
    m_attribOffsets[attribIndex] = m_stride;
    m_stride += s_dataSizes[dataTypeIndex];
    return *this;
}

VertexBuffer& VertexBuffer::SetUsage(BufferUsage usage)
{
    assert(m_hasBuilt);
    m_usage = usage;
    return *this;
}

VertexBuffer& VertexBuffer::SetVertexData(Buffer&& buffer)
{
    assert(m_hasBuilt);
    m_vertexData = std::move(buffer);
    return *this;
}

void VertexBuffer::Build()
{
    assert(m_hasBuilt);
    m_vertexCount = m_vertexData.Size() / m_stride;
    Build_Impl();
}
