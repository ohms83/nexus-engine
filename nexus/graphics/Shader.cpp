//
//  Shader.cpp
//  Nexus
//
//  Created by nuttachai on 21/10/21.
//

#include "Shader.hpp"

USING_NAMESPACE_NXS;

void Shader::initWithData(const Data& vertexShaderData, const Data& fragmentShaderData)
{
    initWithSource(vertexShaderData.get(), fragmentShaderData.get());
}

uint32_t Shader::setUniform(const Uniform& uniform)
{
    uint32_t location = 0;
    
    const auto itr = _uniforms.find(uniform);
    if (itr != _uniforms.end())
    {
        location = findUniform(uniform.name);
        if (location > 0) {
            _uniforms.emplace(uniform, location);
        }
    }
    else
    {
        location = itr->second;
    }
    
    if (location > 0) {
        setUniformImpl(location, uniform.type, uniform.values);
    }
    return location;
}

void Shader::updateUniform(uint32_t location, float* value)
{
    auto itr = _uniforms.begin();
    while (itr != _uniforms.end())
    {
        if (itr->second == location) {
            break;
        }
        ++itr;
    }
    
    if (itr != _uniforms.end())
    {
        const Uniform& uniform = itr->first;
        setUniformImpl(location, uniform.type, value);
    }
}
