//
// Created by nutta on 7/6/2025.
//

#include <nexus/graphics/opengl/GLIndexBuffer.h>

#include "nexus/graphics/opengl/GLGpuBuffer.h"
#include "nexus/graphics/opengl/NxsGL.h"

USING_NAMESPACE_NXS;

GLIndexBuffer::~GLIndexBuffer()
{
    glDeleteBuffers(1, &m_handle);
    CHECK_GL_ERROR();
}

void GLIndexBuffer::Bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_handle);
    CHECK_GL_ERROR();
}

void GLIndexBuffer::Unbind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    CHECK_GL_ERROR();
}

uint32 GLIndexBuffer::Alloc()
{
    glGenBuffers(1, &m_handle);
    CHECK_GL_ERROR();
    return m_handle;
}

void GLIndexBuffer::Build_Impl()
{
    const auto glUsage = NxsBufferUsageToGLenum(m_usage);
    const auto bufferSize = CAST<GLsizeiptr>(sizeof(uint32) * m_indices.size());
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferSize, m_indices.data(), glUsage);
    CHECK_GL_ERROR();
}
