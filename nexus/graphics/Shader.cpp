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
