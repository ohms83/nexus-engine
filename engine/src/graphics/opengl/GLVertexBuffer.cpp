//
// Created by nutta on 7/4/2025.
//

#include <format>
#include <nexus/graphics/opengl/GLVertexBuffer.h>

#include "graphics/opengl/GLGpuBuffer.h"

USING_NAMESPACE_NXS;

std::atomic<uint32_t> GLVertexBuffer::s_bindingBuffer(0);

GLVertexBuffer::~GLVertexBuffer()
{
    if (IsBinding()) Unbind();
    if (m_handle) CALL_GL_FUNC(glDeleteVertexArrays(1, &m_handle));
    if (m_vbo) CALL_GL_FUNC(glDeleteBuffers(1, &m_vbo));
    for (const auto v : m_instanceVBOs)
    {
        if (v) CALL_GL_FUNC(glDeleteBuffers(1, &v));
    }
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
    CALL_GL_FUNC(glBindBuffer(GL_ARRAY_BUFFER, m_vbo));
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

    uint64_t offset = 0;
    for (const auto& v : m_attributes)
    {
        const auto attribIndex = v.attribIndex >= 0 ? v.attribIndex : INT_CAST(v.type);
        const auto glDataType = GL::NxsDataToGLenum(v.dataType);
        CALL_GL_FUNC(glVertexAttribPointer(
            attribIndex,
            v.numElements,
            glDataType,
            GL_FALSE,
            INT_CAST(m_stride),
            R_CAST<const void *>(offset)));
        CALL_GL_FUNC(glEnableVertexAttribArray(attribIndex));

        offset += v.numElements * NxsDataTypeSize(v.dataType);
    }

    // Unbind VBO (it's safe to unbind after glVertexAttribPointer calls,
    // as VAO stores the VBO binding for each attribute)
    CALL_GL_FUNC(glBindBuffer(GL_ARRAY_BUFFER, 0));
    // Unbind VAO
    CALL_GL_FUNC(glBindVertexArray(0));

    // Build and attach instance buffers to VAO: instance streams declared in VertexBuffer
    for (const auto& s : m_instanceStreams)
    {
        GLuint instanceVBO = 0;
        CALL_GL_FUNC(glGenBuffers(1, &instanceVBO));
        CALL_GL_FUNC(glBindBuffer(GL_ARRAY_BUFFER, instanceVBO));
        CALL_GL_FUNC(glBufferData(GL_ARRAY_BUFFER, CAST<GLsizeiptr>(s.buffer->Size()), s.buffer->Data(), NxsBufferUsageToGLenum(s.usage)));

        // Bind the VAO to register attribute pointers that reference this VBO
        CALL_GL_FUNC(glBindVertexArray(m_handle));
        uint64_t ioffset = 0;
        for (const auto& a : s.attributes)
        {
            const auto attribIndex = a.attribIndex >= 0 ? a.attribIndex : INT_CAST(a.type);
            const auto glDataType = GL::NxsDataToGLenum(a.dataType);
            CALL_GL_FUNC(glVertexAttribPointer(
                attribIndex,
                a.numElements,
                glDataType,
                GL_FALSE,
                INT_CAST(s.stride),
                R_CAST<const void*>(ioffset)));
            CALL_GL_FUNC(glEnableVertexAttribArray(attribIndex));
            if (a.divisor > 0)
            {
                CALL_GL_FUNC(glVertexAttribDivisor(attribIndex, a.divisor));
            }
            ioffset += a.numElements * NxsDataTypeSize(a.dataType);
        }
        // Unbind instance VBO
        CALL_GL_FUNC(glBindBuffer(GL_ARRAY_BUFFER, 0));
        CALL_GL_FUNC(glBindVertexArray(0));
        m_instanceVBOs.push_back(instanceVBO);
    }

    // Runtime-attached instance buffers are handled by AttachInstanceStream and immediately registered
}

uint32_t GLVertexBuffer::Alloc()
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
    for (const auto v : m_instanceVBOs)
    {
        if (v) CALL_GL_FUNC(glDeleteBuffers(1, &v));
    }
    m_instanceVBOs.clear();
}

VertexBuffer& GLVertexBuffer::AttachInstanceStream(Ref<IBuffer> instanceData, const std::vector<VertexAttribute>& attributes, BufferUsage usage)
{
    VertexInstanceStream s; s.buffer = instanceData; s.attributes = attributes; s.usage = usage; s.stride = 0; for (const auto &a : attributes) s.stride += NxsDataTypeSize(a.dataType) * a.numElements;
    m_instanceStreamsLocal.push_back(std::move(s));
    const auto& si = m_instanceStreamsLocal.back();

    GLuint instanceVBO = 0;
    CALL_GL_FUNC(glGenBuffers(1, &instanceVBO));
    CALL_GL_FUNC(glBindBuffer(GL_ARRAY_BUFFER, instanceVBO));
    CALL_GL_FUNC(glBufferData(GL_ARRAY_BUFFER, CAST<GLsizeiptr>(si.buffer->Size()), si.buffer->Data(), NxsBufferUsageToGLenum(si.usage)));

    CALL_GL_FUNC(glBindVertexArray(m_handle));
    uint64_t ioffset_local = 0;
    for (const auto& a : si.attributes)
    {
        const auto attribIndex = a.attribIndex >= 0 ? a.attribIndex : INT_CAST(a.type);
        const auto glDataType = GL::NxsDataToGLenum(a.dataType);
        CALL_GL_FUNC(glVertexAttribPointer(
            attribIndex,
            a.numElements,
            glDataType,
            GL_FALSE,
            INT_CAST(si.stride),
            R_CAST<const void*>(ioffset_local)));
        CALL_GL_FUNC(glEnableVertexAttribArray(attribIndex));
        if (a.divisor > 0)
        {
            CALL_GL_FUNC(glVertexAttribDivisor(attribIndex, a.divisor));
        }
        ioffset_local += a.numElements * NxsDataTypeSize(a.dataType);
    }
    CALL_GL_FUNC(glBindBuffer(GL_ARRAY_BUFFER, 0));
    CALL_GL_FUNC(glBindVertexArray(0));

    m_instanceVBOs.push_back(instanceVBO);
    return *this;
}

void GLVertexBuffer::DetachInstanceStreams()
{
    for (const auto v : m_instanceVBOs)
    {
        if (v) CALL_GL_FUNC(glDeleteBuffers(1, &v));
    }
    m_instanceVBOs.clear();
    m_instanceStreamsLocal.clear();
}
