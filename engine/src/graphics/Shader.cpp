//
// Created by nutta on 6/21/2025.
//

#include "nexus/graphics/Shader.h"
#include "nexus/core/LogDispatcher.h"

USING_NAMESPACE_NXS;

DEFINE_LOG(Shader);

bool Shader::Compile(
    RenderingInterface& renderingInterface,
    const std::string& vertex,
    const std::string& fragment)
{
    m_gpuProgram.reset(renderingInterface.CreateGpuProgram());
    m_gpuProgram->BeginCompile()
        .AddSource(vertex, GpuProgram::Type::Vertex)
        .AddSource(fragment, GpuProgram::Type::Fragment)
    .Compile();

    return m_gpuProgram->GetHandle() != 0;
}