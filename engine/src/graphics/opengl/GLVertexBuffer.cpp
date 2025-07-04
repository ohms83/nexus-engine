//
// Created by nutta on 7/4/2025.
//

#include <nexus/graphics/opengl/GLVertexBuffer.h>

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

void GLVertexBuffer::Build_Impl()
{
    // TODO: Update to AZDO (Almost Zero Driver Overhead) workflow
    // https://www.reddit.com/r/opengl/comments/18rkgg3/one_vao_for_multiple_vbos/
    // https://github.com/fendevel/Guide-to-Modern-OpenGL-Functions#glbuffer
    glGenVertexArrays(1, &m_vao);
    CHECK_GL_ERROR();
    glBindVertexArray(m_vao);
    CHECK_GL_ERROR();

    GLenum usage = GL_STATIC_DRAW;
    switch (m_usage)
    {
    case BufferUsage::Static:
        usage = GL_STATIC_DRAW;
        break;
    case BufferUsage::Dynamic:
        usage = GL_DYNAMIC_DRAW;
        break;
    case BufferUsage::Streaming:
        usage = GL_STREAM_DRAW;
        break;
    default:
        assert(false);
    }

    // Create VBO
    glGenBuffers(1, &m_vbo);
    CHECK_GL_ERROR();
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    CHECK_GL_ERROR();
    glBufferData(GL_ARRAY_BUFFER, m_vertexData.Size(), m_vertexData.Get(), usage);
    CHECK_GL_ERROR();
}
