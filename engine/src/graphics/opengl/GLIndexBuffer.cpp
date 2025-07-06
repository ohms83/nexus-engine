//
// Created by nutta on 7/6/2025.
//

#include <nexus/graphics/opengl/GLIndexBuffer.h>

#include "nexus/graphics/opengl/GLGpuBuffer.h"
#include "nexus/graphics/opengl/NxsGL.h"

USING_NAMESPACE_NXS;

GLIndexBuffer::GLIndexBuffer(std::vector<uint32>&& indices, BufferUsage usage)
    : IndexBuffer(std::move(indices), usage)
{
    glGenBuffers(1, &m_ebo);
    CHECK_GL_ERROR();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    CHECK_GL_ERROR();
    const auto glUsage = NxsBufferUsageToGLenum(usage);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32) * indices.size(), indices.data(), glUsage);
    CHECK_GL_ERROR();
}

GLIndexBuffer::~GLIndexBuffer()
{
    glDeleteBuffers(1, &m_ebo);
    CHECK_GL_ERROR();
}

void GLIndexBuffer::Bind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
}
