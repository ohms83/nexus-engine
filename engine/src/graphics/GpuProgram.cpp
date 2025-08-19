

#include "nexus/graphics/GpuProgram.h"
#include "nexus/core/LogDispatcher.h"

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