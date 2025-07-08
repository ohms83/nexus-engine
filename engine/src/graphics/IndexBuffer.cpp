//
// Created by nutta on 7/7/2025.
//
#include <nexus/graphics/IndexBuffer.h>

USING_NAMESPACE_NXS;

IndexBuffer& IndexBuffer::Begin()
{
    assert(!m_hasBuilt && m_handle == 0);
    m_hasBuilt = true;
    m_handle = Alloc();
    Bind();
    return *this;
}

IndexBuffer& IndexBuffer::SetIndices(uint32* indices, const size_t num)
{
    assert(m_hasBuilt);
    m_indices.reserve(num);
    m_indices.assign(indices, indices + num);
    return *this;
}

IndexBuffer& IndexBuffer::SetIndices(std::vector<uint32>&& indices)
{
    assert(m_hasBuilt);
    m_indices = std::move(indices);
    return *this;
}

IndexBuffer& IndexBuffer::SetUsage(const BufferUsage usage)
{
    assert(m_hasBuilt);
    m_usage = usage;
    return *this;
}

IndexBuffer& IndexBuffer::SetDrawMode(const DrawMode mode)
{
    assert(m_hasBuilt);
    m_drawMode = mode;
    return *this;
}

void IndexBuffer::Build()
{
    assert(m_hasBuilt);
    Build_Impl();
    Unbind();
}

uint32 IndexBuffer::NumPolygons() const
{
    const uint32 numIndices = m_indices.size();
    switch (m_drawMode)
    {
    case DrawMode::Line:
        return numIndices / 2;
    case DrawMode::Triangle:
        return numIndices / 3;
    case DrawMode::Point:
        return numIndices;
    case DrawMode::LineLoop:
    case DrawMode::LineStrip:
        return numIndices - 1;
    case DrawMode::TriangleFan:
    case DrawMode::TriangleStrip:
        return numIndices - 2;
    case DrawMode::Quad:
        return numIndices / 4;
    default:
        assert(false);
        break;
    }
    return 0;
}
