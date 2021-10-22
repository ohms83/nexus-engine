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

void GLVertexBuffer::initImpl(const VertexBufferCreateInfo& info)
{
    glGenVertexArrays(1, &_vao);
    CHECK_GL_ERROR();
    glBindVertexArray(_vao);
    CHECK_GL_ERROR();
    
    const GLsizei stride = (GLsizei)_description.getVertexSize();

    // Create VBO
    const uint32_t size = stride * _vertexCount;
    glGenBuffers(1, &_vbo);
    CHECK_GL_ERROR();
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    CHECK_GL_ERROR();
    glBufferData(GL_ARRAY_BUFFER, size, info.vertices, _usage);
    CHECK_GL_ERROR();

    // Create EBO
    if (_indexCount > 0 && info.indices != nullptr)
    {
        glGenBuffers(1, &_ebo);
        CHECK_GL_ERROR();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
        CHECK_GL_ERROR();
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * _indexCount, info.indices, _usage);
        CHECK_GL_ERROR();
    }
    
    GLuint attribIndex = 0;

    if (_description.positionComponentCount > 0)
    {
        glVertexAttribPointer(attribIndex, _description.positionComponentCount, GL_FLOAT, GL_FALSE, stride, (GLvoid*)_description.positionOffset);
        CHECK_GL_ERROR();
        glEnableVertexAttribArray(attribIndex);
        CHECK_GL_ERROR();
        
        ++attribIndex;
    }
    
    if (_description.normalComponentCount > 0)
    {
        glVertexAttribPointer(attribIndex, _description.normalComponentCount, GL_FLOAT, GL_FALSE, stride, (GLvoid*)_description.normalOffset);
        CHECK_GL_ERROR();
        glEnableVertexAttribArray(attribIndex);
        CHECK_GL_ERROR();
        
        ++attribIndex;
    }
    
    if (_description.colorComponentCount > 0)
    {
        glVertexAttribPointer(attribIndex, _description.colorComponentCount, GL_FLOAT, GL_FALSE, stride, (GLvoid*)_description.colorOffset);
        CHECK_GL_ERROR();
        glEnableVertexAttribArray(attribIndex);
        CHECK_GL_ERROR();
        
        ++attribIndex;
    }

    for (uint32_t i = 0; i < _description.textureUnit; ++i)
    {
        glVertexAttribPointer(attribIndex, _description.texCoordComponentCount, GL_FLOAT, GL_FALSE, stride, (GLvoid*)_description.texCoordOffsets[i]);
        CHECK_GL_ERROR();
        glEnableVertexAttribArray(attribIndex);
        CHECK_GL_ERROR();
        
        ++attribIndex;
    }

    glBindVertexArray(0);
    CHECK_GL_ERROR();
}
