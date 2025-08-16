//
// Created by nutta on 6/21/2025.
//

#include "nexus/graphics/Shader.h"
#include "nexus/core/LogDispatcher.h"

USING_NAMESPACE_NXS;

DEFINE_LOG(Shader);

Shader& Shader::BeginCompile()
{
    NXS_ASSERT_MSG(m_shaderID == 0 && !m_compiling, std::format("Shader::BeginCompile() was already called once."));
    m_compiling = true;
    m_shaderID = Alloc();
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
