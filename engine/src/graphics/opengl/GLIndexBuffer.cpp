//
// Created by nutta on 7/6/2025.
//

#include <nexus/graphics/opengl/GLIndexBuffer.h>

#include "nexus/graphics/opengl/GLGpuBuffer.h"
#include "nexus/graphics/opengl/NxsGL.h"

USING_NAMESPACE_NXS;

GLIndexBuffer::~GLIndexBuffer()
{
    CALL_GL_FUNC(glDeleteBuffers(1, &m_handle));
}

void GLIndexBuffer::Bind() const
{
    CALL_GL_FUNC(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_handle));
}

void GLIndexBuffer::Unbind() const
{
    CALL_GL_FUNC(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

uint32 GLIndexBuffer::Alloc()
{
    CALL_GL_FUNC(glGenBuffers(1, &m_handle));
    return m_handle;
}

void GLIndexBuffer::Release()
{
    CALL_GL_FUNC(glDeleteBuffers(1, &m_handle));
}

void GLIndexBuffer::Build_Impl()
{
    const auto gl_usage = NxsBufferUsageToGLenum(m_usage);
    const auto bufferSize = CAST<GLsizeiptr>(sizeof(uint32) * m_indices.size());
    CALL_GL_FUNC(glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferSize, m_indices.data(), gl_usage));
}
