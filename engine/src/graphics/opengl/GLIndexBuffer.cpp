//
// Created by nutta on 7/6/2025.
//

#include <nexus/graphics/opengl/GLIndexBuffer.h>

#include "nexus/graphics/opengl/GLGpuBuffer.h"
#include "nexus/graphics/opengl/NxsGL.h"

USING_NAMESPACE_NXS;

std::atomic<uint32> GLIndexBuffer::s_bindingBuffer(0);

GLIndexBuffer::~GLIndexBuffer()
{
    if (IsBinding()) Unbind();
    CALL_GL_FUNC(glDeleteBuffers(1, &m_handle));
}

void GLIndexBuffer::Bind() const
{
    CALL_GL_FUNC(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_handle));
    s_bindingBuffer.store(m_handle);
}

void GLIndexBuffer::Unbind() const
{
    CALL_GL_FUNC(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    if (IsBinding()) s_bindingBuffer.store(0);
}

bool GLIndexBuffer::IsBinding() const
{
    return m_handle != 0 && s_bindingBuffer.load() == m_handle;
}

void GLIndexBuffer::CopyData(const void* data, size_t bytes, size_t offset)
{
    IndexBuffer::CopyData(data, bytes, offset);
    NXS_ASSERT_MSG(IsBinding(), std::format("Invalid operation. The buffer is unbound."));
    CALL_GL_FUNC(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, bytes, data));
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
    const auto bufferSize = CAST<GLsizeiptr>(m_indexData->Size());
    CALL_GL_FUNC(glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferSize, m_indexData->Data(), gl_usage));
}
