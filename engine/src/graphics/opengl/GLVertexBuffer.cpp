//
// Created by nutta on 7/4/2025.
//

#include <nexus/graphics/opengl/GLVertexBuffer.h>

#include "graphics/opengl/GLGpuBuffer.h"

USING_NAMESPACE_NXS;

GLVertexBuffer::GLVertexBuffer()
    : m_vbo(0)
    , m_vao(0)
{
}

GLVertexBuffer::~GLVertexBuffer()
{
    if (m_vao){
        glDeleteVertexArrays(1, &m_vao);
        CHECK_GL_ERROR();
    }
    if (m_vbo) {
        glDeleteBuffers(1, &m_vbo);
        CHECK_GL_ERROR();
    }
}

VertexBuffer& GLVertexBuffer::Begin(Buffer&& vertexData, BufferUsage usage)
{
    VertexBuffer::Begin(std::move(vertexData), usage);

    glGenVertexArrays(1, &m_vao);
    CHECK_GL_ERROR();
    glBindVertexArray(m_vao);
    CHECK_GL_ERROR();
    // Create a VBO
    glGenBuffers(1, &m_vbo);
    CHECK_GL_ERROR();
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    CHECK_GL_ERROR();

    GLenum glUsage = NxsBufferUsageToGLenum(usage);
    glBufferData(GL_ARRAY_BUFFER, m_vertexData.Size(), m_vertexData.Get(), glUsage);
    CHECK_GL_ERROR();

    return *this;
}

VertexBuffer& GLVertexBuffer::Begin(uint8* data, size_t size, BufferUsage usage)
{
    VertexBuffer::Begin(data, size, usage);

    glGenVertexArrays(1, &m_vao);
    CHECK_GL_ERROR();
    glBindVertexArray(m_vao);
    CHECK_GL_ERROR();
    // Create a VBO
    glGenBuffers(1, &m_vbo);
    CHECK_GL_ERROR();
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    CHECK_GL_ERROR();

    GLenum glUsage = NxsBufferUsageToGLenum(usage);
    glBufferData(GL_ARRAY_BUFFER, m_vertexData.Size(), m_vertexData.Get(), glUsage);
    CHECK_GL_ERROR();

    return *this;
}

void GLVertexBuffer::Build_Impl()
{
    // TODO: Update to AZDO (Almost Zero Driver Overhead) workflow
    // https://www.reddit.com/r/opengl/comments/18rkgg3/one_vao_for_multiple_vbos/
    // https://github.com/fendevel/Guide-to-Modern-OpenGL-Functions#glbuffer

    size_t offset = 0;
    for (const auto& [type, dataType, numElements] : m_attributes)
    {
        const auto attribIndex = INT_CAST(type);
        const auto glDataType = GL::NxsDataToGLenum(dataType);
        glVertexAttribPointer(attribIndex, offset, glDataType, GL_FALSE, m_stride, R_CAST<GLvoid*>(offset));
        CHECK_GL_ERROR();
        glEnableVertexAttribArray(attribIndex);
        CHECK_GL_ERROR();

        offset += NxsDataTypeSize(dataType) * numElements;
    }

    // Unbind VBO (it's safe to unbind after glVertexAttribPointer calls,
    // as VAO stores the VBO binding for each attribute)
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    CHECK_GL_ERROR();
    // Unbind VAO
    glBindVertexArray(0);
    CHECK_GL_ERROR();
}
