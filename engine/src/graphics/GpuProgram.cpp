

#include "graphics/GpuProgram.h"
#include "nexus/debug/LogDispatcher.h"

#include <ranges>

USING_NAMESPACE_NXS;

DEFINE_LOG(GpuProgram);

GpuProgram& GpuProgram::BeginCompile()
{
    NXS_ASSERT_MSG(m_id == 0 && !m_compiling, std::format("GpuProgram::BeginCompile() was already called once."));
    m_compiling = true;
    m_id = Alloc();
    return *this;
}

GpuProgram& GpuProgram::AddSource(const std::string& source, Type GpuProgramType)
{
    assert(m_compiling);
    return *this;
}

void GpuProgram::Compile()
{
    assert(m_compiling);
    m_compiling = false;
}

int32_t GpuProgram::FindUniform(const std::string &name)
{
    if (const auto& itr = m_uniforms.find(name); itr != m_uniforms.end()) return itr->second;

    int32_t location = FindUniform_Internal(name);
    m_uniforms[name] = location;
    return location;
}