//
// Created by nutta on 6/21/2025.
//

#include <nexus/graphics/Shader.h>

USING_NAMESPACE_NXS;

Shader& Shader::BeginCompile()
{
    assert(m_shaderProgram == 0 && !m_compiling);
    m_compiling = true;
    return *this;
}

Shader& Shader::AddSource(const std::string& source, Type shaderType)
{
    assert(m_compiling);
    return *this;
}

void Shader::Compile()
{
    assert(m_compiling);
    m_compiling = false;
}
