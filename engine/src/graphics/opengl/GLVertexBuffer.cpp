//
// Created by nutta on 7/4/2025.
//

#include <format>
#include <nexus/graphics/opengl/GLVertexBuffer.h>

#include "graphics/opengl/GLGpuBuffer.h"

USING_NAMESPACE_NXS;

std::atomic<uint32> GLVertexBuffer::s_bindingBuffer(0);

GLVertexBuffer::~GLVertexBuffer()
{
    if (IsBinding()) Unbind();
    if (m_handle) CALL_GL_FUNC(glDeleteVertexArrays(1, &m_handle));
    if (m_vbo) CALL_GL_FUNC(glDeleteBuffers(1, &m_vbo));
}

VertexBuffer& GLVertexBuffer::Begin()
{
    VertexBuffer::Begin();
    CALL_GL_FUNC(glBindVertexArray(m_handle));
    return *this;
}

void GLVertexBuffer::Bind() const
{
    CALL_GL_FUNC(glBindVertexArray(m_handle));
    s_bindingBuffer.store(m_handle);
}

void GLVertexBuffer::Unbind() const
{
    CALL_GL_FUNC(glBindVertexArray(0));
    if (IsBinding()) s_bindingBuffer.store(0);
}

bool GLVertexBuffer::IsBinding() const
{
    return m_handle != 0 && s_bindingBuffer.load() == m_handle;
}

void GLVertexBuffer::CopyData(const void* data, size_t bytes, size_t offset)
{
    NXS_ASSERT_MSG(IsBinding(), std::format("Invalid operation. The buffer is unbound."));
    CALL_GL_FUNC(glBufferSubData(GL_ARRAY_BUFFER, offset, bytes, data));
}

void GLVertexBuffer::Build_Impl()
{
    // TODO: Update to AZDO (Almost Zero Driver Overhead) workflow
    // https://www.reddit.com/r/opengl/comments/18rkgg3/one_vao_for_multiple_vbos/
    // https://github.com/fendevel/Guide-to-Modern-OpenGL-Functions#glbuffer

    // Create a VBO
    CALL_GL_FUNC(glGenBuffers(1, &m_vbo));
    CALL_GL_FUNC(glBindBuffer(GL_ARRAY_BUFFER, m_vbo));

    const GLenum glUsage = NxsBufferUsageToGLenum(m_usage);
    CALL_GL_FUNC(glBufferData(GL_ARRAY_BUFFER, m_vertices->Size(), m_vertices->Data(), glUsage));

    GLint offset = 0;
    for (const auto& [type, dataType, numElements] : m_attributes)
    {
        const auto attribIndex = INT_CAST(type);
        const auto glDataType = GL::NxsDataToGLenum(dataType);
        CALL_GL_FUNC(glVertexAttribPointer(
            attribIndex,
            numElements,
            glDataType,
            GL_FALSE,
            INT_CAST(m_stride),
            R_CAST<const void *>(offset)));
        CALL_GL_FUNC(glEnableVertexAttribArray(attribIndex));

        offset += numElements * NxsDataTypeSize(dataType);
    }

    // Unbind VBO (it's safe to unbind after glVertexAttribPointer calls,
    // as VAO stores the VBO binding for each attribute)
    CALL_GL_FUNC(glBindBuffer(GL_ARRAY_BUFFER, 0));
    // Unbind VAO
    CALL_GL_FUNC(glBindVertexArray(0));
}

uint32 GLVertexBuffer::Alloc()
{
    CALL_GL_FUNC(glGenVertexArrays(1, &m_handle));
    return m_handle;
}

void GLVertexBuffer::Release()
{
    if (m_handle) {
        CALL_GL_FUNC(glDeleteVertexArrays(1, &m_handle));
    }
    if (m_vbo) {
        CALL_GL_FUNC(glDeleteBuffers(1, &m_vbo));
    }
}
