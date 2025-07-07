//
// Created by nutta on 7/4/2025.
//

#include <nexus/graphics/opengl/GLVertexBuffer.h>

#include "graphics/opengl/GLGpuBuffer.h"

USING_NAMESPACE_NXS;

GLVertexBuffer::~GLVertexBuffer()
{
    if (m_handle) {
        glDeleteVertexArrays(1, &m_handle);
        CHECK_GL_ERROR();
    }
    if (m_vbo) {
        glDeleteBuffers(1, &m_vbo);
        CHECK_GL_ERROR();
    }
}

VertexBuffer& GLVertexBuffer::Begin()
{
    VertexBuffer::Begin();

    glGenVertexArrays(1, &m_handle);
    CHECK_GL_ERROR();
    glBindVertexArray(m_handle);
    CHECK_GL_ERROR();
    return *this;
}

void GLVertexBuffer::Bind() const
{
    glBindVertexArray(m_handle);
    CHECK_GL_ERROR();
}

void GLVertexBuffer::Unbind() const
{
    glBindVertexArray(0);
    CHECK_GL_ERROR();
}

void GLVertexBuffer::Build_Impl()
{
    // TODO: Update to AZDO (Almost Zero Driver Overhead) workflow
    // https://www.reddit.com/r/opengl/comments/18rkgg3/one_vao_for_multiple_vbos/
    // https://github.com/fendevel/Guide-to-Modern-OpenGL-Functions#glbuffer

    // Create a VBO
    glGenBuffers(1, &m_vbo);
    CHECK_GL_ERROR();
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    CHECK_GL_ERROR();

    const GLenum glUsage = NxsBufferUsageToGLenum(m_usage);
    glBufferData(GL_ARRAY_BUFFER, m_bufferSize, m_vertices.get(), glUsage);
    CHECK_GL_ERROR();

    GLint offset = 0;
    for (const auto& [type, dataType, numElements] : m_attributes)
    {
        const auto attribIndex = INT_CAST(type);
        const auto glDataType = GL::NxsDataToGLenum(dataType);
        glVertexAttribPointer(
            attribIndex,
            numElements,
            glDataType,
            GL_FALSE,
            INT_CAST(m_stride),
            R_CAST<const void *>(offset));
        CHECK_GL_ERROR();
        glEnableVertexAttribArray(attribIndex);
        CHECK_GL_ERROR();

        offset += numElements * NxsDataTypeSize(dataType);
    }

    // Unbind VBO (it's safe to unbind after glVertexAttribPointer calls,
    // as VAO stores the VBO binding for each attribute)
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    CHECK_GL_ERROR();
    // Unbind VAO
    glBindVertexArray(0);
    CHECK_GL_ERROR();
}
