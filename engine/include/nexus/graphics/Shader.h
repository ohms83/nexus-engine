//
// Created by nutta on 6/21/2025.
//

#pragma once

#include "nexus/NxsCommon.h"

#include "nexus/core/Resource.h"
#include "GpuProgram.h"
#include "RenderingInterface.h"

#include <string>

NXS_NAMESPACE
{
    class Shader : public Resource
    {
    public:
        Shader(std::string path, const uint32 id) : Resource(std::move(path), id) {}

        MAYBE_UNUSED bool Compile(RenderingInterface& renderingInterface,
            const std::string& vertex, const std::string& fragment);

        NODISCARD Ref<GpuProgram> GetGpuProgram() const
        {
            return m_gpuProgram;
        }

    protected:
        Ref<GpuProgram> m_gpuProgram;
        std::string m_vertexShaderSource;
        std::string m_fragmentShaderSource;
    };
}