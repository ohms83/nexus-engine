//
//  RenderCommand.cpp
//  Nexus
//
//  Created by nuttachai on 23/10/21.
//

#include "RenderCommand.hpp"

USING_NAMESPACE_NXS;

RenderCommand::RenderCommand()
{
}

RenderCommand::~RenderCommand()
{
}

void RenderCommand::addVertexBuffer(const VertexBuffer* vertexBuffer)
{
    if (vertexBuffer) {
        _bufferList.push_back(vertexBuffer);
    }
}

void RenderCommand::setShader(const Shader* shader)
{
    _shader = shader;
}

