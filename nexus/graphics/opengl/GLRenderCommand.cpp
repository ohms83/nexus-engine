//
//  GLRenderCommand.cpp
//  Nexus
//
//  Created by nuttachai on 23/10/21.
//

#include "GLRenderCommand.hpp"

#include "GLVertexBuffer.hpp"
#include "GLShader.hpp"

USING_NAMESPACE_NXS;

GLRenderCommand::GLRenderCommand()
{
    
}

GLRenderCommand::~GLRenderCommand()
{
    
}

void GLRenderCommand::execute() const
{
    const GLShader* glShader = dynamic_cast<const GLShader*>(_shader);
    if (!glShader) {
        return;
    }
    
    glUseProgram(glShader->getShaderProgram());
    CHECK_GL_ERROR();
    
    for (const auto vertexBuffer : _bufferList)
    {
        const GLVertexBuffer* glVertexBuffer = dynamic_cast<const GLVertexBuffer*>(vertexBuffer);
        if (!glVertexBuffer) {
            continue;
        }
        
        const VertexBufferInfo& bufferInfo = glVertexBuffer->getInfo();
        
        glBindVertexArray(glVertexBuffer->getVAO());
        CHECK_GL_ERROR();
        
        if (bufferInfo.indexCount > 0) {
            glDrawElements(bufferInfo.primitiveType, bufferInfo.indexCount, GL_UNSIGNED_INT, 0);
        }
        else {
            glDrawArrays(bufferInfo.primitiveType, 0, bufferInfo.vertexCount);
        }
        CHECK_GL_ERROR();
    }
}
