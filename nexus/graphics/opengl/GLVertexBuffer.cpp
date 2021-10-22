//
//  GLVertexBuffer.cpp
//  Nexus
//
//  Created by nuttachai on 21/10/21.
//

#include "GLVertexBuffer.hpp"

USING_NAMESPACE_NXS;

GLVertexBuffer::~GLVertexBuffer()
{
    if (_vao) {
        glDeleteVertexArrays(1, &_vao);
        CHECK_GL_ERROR();
    }
    if (_vbo) {
        glDeleteBuffers(1, &_vbo);
        CHECK_GL_ERROR();
    }
    if (_ebo) {
        glDeleteBuffers(1, &_ebo);
        CHECK_GL_ERROR();
    }
}

void GLVertexBuffer::initImpl(const VertexBufferCreateInfo& createInfo)
{
    glGenVertexArrays(1, &_vao);
    CHECK_GL_ERROR();
    glBindVertexArray(_vao);
    CHECK_GL_ERROR();
    
    const VertexDescription& description = _info.description;
    const GLsizei stride = (GLsizei)description.getVertexSize();

    // Create VBO
    const uint32_t size = stride * _info.vertexCount;
    glGenBuffers(1, &_vbo);
    CHECK_GL_ERROR();
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    CHECK_GL_ERROR();
    glBufferData(GL_ARRAY_BUFFER, size, createInfo.vertices, _info.usage);
    CHECK_GL_ERROR();

    // Create EBO
    if (_info.indexCount > 0 && createInfo.indices != nullptr)
    {
        glGenBuffers(1, &_ebo);
        CHECK_GL_ERROR();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
        CHECK_GL_ERROR();
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * _info.indexCount, createInfo.indices, _info.usage);
        CHECK_GL_ERROR();
    }
    
    GLuint attribIndex = 0;

    if (description.positionComponentCount > 0)
    {
        glVertexAttribPointer(attribIndex, description.positionComponentCount, GL_FLOAT, GL_FALSE, stride, (GLvoid*)description.positionOffset);
        CHECK_GL_ERROR();
        glEnableVertexAttribArray(attribIndex);
        CHECK_GL_ERROR();
        
        ++attribIndex;
    }
    
    if (description.normalComponentCount > 0)
    {
        glVertexAttribPointer(attribIndex, description.normalComponentCount, GL_FLOAT, GL_FALSE, stride, (GLvoid*)description.normalOffset);
        CHECK_GL_ERROR();
        glEnableVertexAttribArray(attribIndex);
        CHECK_GL_ERROR();
        
        ++attribIndex;
    }
    
    if (description.colorComponentCount > 0)
    {
        glVertexAttribPointer(attribIndex, description.colorComponentCount, GL_FLOAT, GL_FALSE, stride, (GLvoid*)description.colorOffset);
        CHECK_GL_ERROR();
        glEnableVertexAttribArray(attribIndex);
        CHECK_GL_ERROR();
        
        ++attribIndex;
    }

    for (uint32_t i = 0; i < description.textureUnit; ++i)
    {
        glVertexAttribPointer(attribIndex, description.texCoordComponentCount, GL_FLOAT, GL_FALSE, stride, (GLvoid*)description.texCoordOffsets[i]);
        CHECK_GL_ERROR();
        glEnableVertexAttribArray(attribIndex);
        CHECK_GL_ERROR();
        
        ++attribIndex;
    }

    glBindVertexArray(0);
    CHECK_GL_ERROR();
}
