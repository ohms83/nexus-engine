//
// Created by nutta on 7/7/2025.
//
#include <nexus/graphics/IndexBuffer.h>

#include "core/Logger.h"

USING_NAMESPACE_NXS;

DEFINE_LOG(IndexBuffer);

IndexBuffer& IndexBuffer::Begin()
{
    assert(!m_hasBuilt && m_handle == 0);
    m_hasBuilt = true;
    m_handle = Alloc();
    Bind();
    return *this;
}

IndexBuffer& IndexBuffer::SetIndices(uint32* indices, const size_t num, FrontFace frontFace)
{
    NXS_ASSERT_MSG(m_hasBuilt, "Begin function hasn't been called yet.");
    m_indices.reserve(num);
    m_indices.assign(indices, indices + num);
    m_frontFace = frontFace;
    return *this;
}

IndexBuffer& IndexBuffer::SetIndices(std::vector<uint32>&& indices, FrontFace frontFace)
{
    NXS_ASSERT_MSG(m_hasBuilt, "Begin function hasn't been called yet.");
    m_indices = std::move(indices);
    m_frontFace = frontFace;
    return *this;
}

IndexBuffer& IndexBuffer::SetUsage(const BufferUsage usage)
{
    NXS_ASSERT_MSG(m_hasBuilt, "Begin function hasn't been called yet.");
    m_usage = usage;
    return *this;
}

IndexBuffer& IndexBuffer::SetDrawMode(const DrawMode mode)
{
    NXS_ASSERT_MSG(m_hasBuilt, "Begin function hasn't been called yet.");
    m_drawMode = mode;
    return *this;
}

void IndexBuffer::Build()
{
    NXS_ASSERT_MSG(m_hasBuilt, "Begin function hasn't been called yet.");
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
        NXS_ASSERT_MSG(false, std::format("Invalid draw mode {}", CAST<int>(m_drawMode)));
        break;
    }
    return 0;
}

void IndexBuffer::ReArrangeIndex(const FrontFace frontFace)
{
    if (m_indices.empty())
    {
        LOG_WARNING(LogIndexBuffer, "The index buffer is empty.");
        return;
    }
    if (frontFace == m_frontFace)
    {
        LOG_WARNING(LogIndexBuffer, "The new front-face is the same as the existing. There's no need to re-arrange");
        return;
    }
    if (m_drawMode != DrawMode::Triangle && m_drawMode != DrawMode::Quad)
    {
        LOG_WARNING(LogIndexBuffer, std::format("Cannot re-arrange indexes of this type {}", CAST<int>(m_drawMode)));
        return;
    }

    auto* indexData = m_indices.data();
    const auto stride = m_drawMode == DrawMode::Triangle ? 3 : 4;
    const auto swapIndex = m_drawMode == DrawMode::Triangle ? 2 : 3;
    for (size_t i = 0; i < NumPolygons(); i++)
    {
        const size_t faceIndex = i * stride;
        std::swap(indexData[faceIndex + 1], indexData[faceIndex + swapIndex]);
    }
}
