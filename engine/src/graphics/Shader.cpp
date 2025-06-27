//
// Created by nutta on 6/21/2025.
//

#include <nexus/graphics/Shader.h>

#include "bx/readerwriter.h"

USING_NAMESPACE_NXS;

Shader::~Shader()
{
    // The vertex and fragment shaders will automatically be destroyed together with the program.
    bgfx::destroy(m_program);
    m_program = BGFX_INVALID_HANDLE;
    m_vertexShader = BGFX_INVALID_HANDLE;
    m_fragmentShader = BGFX_INVALID_HANDLE;
}

void Shader::InitFromCompiledShaders(const std::string& vertexShaderFile, const std::string& fragmentShaderFile)
{
    m_vertexShader = bgfx::createShader(LoadFromMemory(vertexShaderFile));
    m_fragmentShader = bgfx::createShader(LoadFromMemory(fragmentShaderFile));

    // TODO: Set names
    // bgfx::setName(handle, _name.getPtr(), _name.getLength() );

    m_program = bgfx::createProgram(m_vertexShader, m_fragmentShader, true);
}

bgfx::Memory Shader::CompileShader(const std::string& shaderCode)
{

}

const bgfx::Memory* Shader::LoadFromMemory(const std::string& shaderPath)
{
    bx::FileReaderI* reader {};
    if (const bx::FilePath filePath((shaderPath.data())); bx::open(reader, filePath) )
    {
        const uint32_t size = CAST<uint32_t>(bx::getSize(reader));
        const bgfx::Memory* mem = bgfx::alloc(size+1);
        bx::read(reader, mem->data, size, bx::ErrorAssert{});
        bx::close(reader);
        mem->data[mem->size-1] = '\0';
        return mem;
    }
    return nullptr;
}
