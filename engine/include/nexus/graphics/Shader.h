//
// Created by nutta on 6/21/2025.
//

#pragma once

#include <nexus/NxsDefine.h>
#include <string>
#include <bgfx/bgfx.h>

NXS_NAMESPACE
{
    class Shader
    {
    public:
        virtual ~Shader();

        bgfx::ProgramHandle GetHandle() const
        {
            return m_program;
        }

        void InitFromCompiledShaders(const std::string& vertexShaderFile, const std::string& fragmentShaderFile);

    protected:
        static bgfx::Memory* CompileShader(const std::string& shaderCode);
        static const bgfx::Memory* LoadFromMemory(const std::string& shaderPath);

    protected:
        bgfx::ProgramHandle m_program = BGFX_INVALID_HANDLE;
        bgfx::ShaderHandle m_vertexShader = BGFX_INVALID_HANDLE;
        bgfx::ShaderHandle m_fragmentShader = BGFX_INVALID_HANDLE;
    };
}