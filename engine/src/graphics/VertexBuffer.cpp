//
// Created by nutta on 6/27/2025.
//
#include "graphics/VertexBuffer.h"
#include <cstring>

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

VertexBuffer& VertexBuffer::SetVertices(Ref<IBuffer> vertexData)
{
    assert(m_hasBuilt);
    m_vertices = vertexData;
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

VertexBuffer& VertexBuffer::AddInstanceStream(Ref<IBuffer> instanceData, const std::vector<VertexAttribute>& attributes, BufferUsage usage)
{
    assert(m_hasBuilt);
    VertexInstanceStream s;
    s.buffer = instanceData;
    s.attributes = attributes;
    s.usage = usage;
    // compute stride
    uint32_t stride = 0;
    for (const auto& a : attributes) {
        stride += NxsDataTypeSize(a.dataType) * a.numElements;
    }
    s.stride = stride;
    m_instanceStreams.push_back(std::move(s));
    return *this;
}

VertexBuffer& VertexBuffer::AttachInstanceStream(Ref<IBuffer> instanceData, const std::vector<VertexAttribute>& attributes, BufferUsage usage)
{
    assert(m_hasBuilt);
    VertexInstanceStream s;
    s.buffer = instanceData;
    s.attributes = attributes;
    s.usage = usage;
    s.stride = 0;
    for (const auto& a : attributes) s.stride += NxsDataTypeSize(a.dataType) * a.numElements;
    m_instanceStreams.push_back(std::move(s));
    // Platform-specific VAO attachment happens in GLVertexBuffer::AttachInstanceStream_Impl
    return *this;
}

void VertexBuffer::DetachInstanceStreams()
{
    m_instanceStreams.clear();
}

void VertexBuffer::Build()
{
    assert(m_hasBuilt);
    m_vertexCount = m_vertices->Size() / m_stride;
    Build_Impl();
}

NODISCARD uint32_t VertexBuffer::GetAttributeOffset(VertexAttribute::Type type) const
{
    uint32_t offset = 0;
    for (const auto& attr : m_attributes)
    {
        if (attr.type == type)
        {
            return offset;
        }
        offset += NxsDataTypeSize(attr.dataType) * attr.numElements;
    }
    return 0;
}
