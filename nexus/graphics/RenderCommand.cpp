//
//  RenderCommand.cpp
//  Nexus
//
//  Created by nuttachai on 23/10/21.
//

#include "RenderCommand.hpp"
#include "Shader.hpp"

USING_NAMESPACE_NXS;

RenderCommand::RenderCommand()
{
}

RenderCommand::~RenderCommand()
{
}

void RenderCommand::addVertexBuffer(std::shared_ptr<const VertexBuffer> vertexBuffer)
{
    if (vertexBuffer) {
        _bufferList.push_back(vertexBuffer);
    }
}

const std::vector<std::shared_ptr<const VertexBuffer>>& RenderCommand::getBufferList() const
{
    return _bufferList;
}

void RenderCommand::setShader(std::shared_ptr<const Shader> shader)
{
    _shader = shader;
}

std::shared_ptr<const Shader> RenderCommand::getShader() const
{
    return _shader;
}

uint32_t RenderCommand::setUniform(const std::string& name, const glm::vec2& value)
{
    return setUniform(name, value, _vec2Uniforms);
}

uint32_t RenderCommand::setUniform(const std::string& name, const glm::vec3& value)
{
    return setUniform(name, value, _vec3Uniforms);
}

uint32_t RenderCommand::setUniform(const std::string& name, const glm::vec4& value)
{
    return setUniform(name, value, _vec4Uniforms);
}

uint32_t RenderCommand::setUniform(const std::string& name, const glm::mat3& value)
{
    return setUniform(name, value, _mat3Uniforms);
}

uint32_t RenderCommand::setUniform(const std::string& name, const glm::mat4& value)
{
    return setUniform(name, value, _mat4Uniforms);
}
