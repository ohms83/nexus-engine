//
// Created by nutta on 7/6/2025.
//

#include "nexus/graphics/opengl/GLIndexBuffer.h"

#include "nexus/graphics/opengl/GLGpuBuffer.h"
#include "nexus/graphics/opengl/NxsGL.h"

#include "Remotery.h"

USING_NAMESPACE_NXS;

GLIndexBuffer::~GLIndexBuffer()
{
    if (IsBinding()) Unbind();
    CALL_GL_FUNC(glDeleteBuffers(1, &m_handle));
}

IndexBuffer& GLIndexBuffer::Begin()
{
    assert(!m_hasBuilt && m_handle == 0);
    m_hasBuilt = true;
    m_handle = Alloc();
    return *this;
}

void GLIndexBuffer::Bind() const
{
    rmt_ScopedCPUSample(GLIndexBuffer_Bind, 0);
    if (IsBinding()) return;

    CALL_GL_FUNC(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_handle));
}

void GLIndexBuffer::Unbind() const
{
    rmt_ScopedCPUSample(GLIndexBuffer_Unbind, 0);
    if (!IsBinding()) return;

    CALL_GL_FUNC(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

bool GLIndexBuffer::IsBinding() const
{
    GLint binding = 0;
    CALL_GL_FUNC(glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &binding));
    return m_handle != 0 && CAST<uint32_t>(binding) == m_handle;
}

void GLIndexBuffer::CopyData(const void* data, size_t bytes, size_t offset)
{
    IndexBuffer::CopyData(data, bytes, offset);
    GLint previousBinding = 0;
    CALL_GL_FUNC(glGetIntegerv(GL_COPY_WRITE_BUFFER, &previousBinding));
    CALL_GL_FUNC(glBindBuffer(GL_COPY_WRITE_BUFFER, m_handle));
    CALL_GL_FUNC(glBufferSubData(GL_COPY_WRITE_BUFFER, offset, bytes, data));
    CALL_GL_FUNC(glBindBuffer(GL_COPY_WRITE_BUFFER, CAST<GLuint>(previousBinding)));
}

uint32_t GLIndexBuffer::Alloc()
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
    const auto bufferSize = CAST<GLsizeiptr>(m_indexData->Size());
    GLint previousBinding = 0;
    CALL_GL_FUNC(glGetIntegerv(GL_COPY_WRITE_BUFFER, &previousBinding));
    CALL_GL_FUNC(glBindBuffer(GL_COPY_WRITE_BUFFER, m_handle));
    CALL_GL_FUNC(glBufferData(GL_COPY_WRITE_BUFFER, bufferSize, m_indexData->Data(), gl_usage));
    CALL_GL_FUNC(glBindBuffer(GL_COPY_WRITE_BUFFER, CAST<GLuint>(previousBinding)));
}
