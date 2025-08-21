//
// Created by nutta on 6/21/2025.
//

#include "graphics/Shader.h"
#include "core/LogDispatcher.h"
#include "core/StringUtil.h"

#include <fstream>
#include <string_view>
#include <ranges>

USING_NAMESPACE_NXS;

DEFINE_LOG(Shader);

bool Shader::CompileFromSource(
    RenderingInterface& renderingInterface,
    std::string&& vertexShaderSource,
    std::string&& fragmentShaderSource)
{
    m_gpuProgram.reset(renderingInterface.CreateGpuProgram());
    m_gpuProgram->BeginCompile()
        .AddSource(vertexShaderSource, GpuProgram::Type::Vertex)
        .AddSource(fragmentShaderSource, GpuProgram::Type::Fragment)
    .Compile();

    if (m_gpuProgram->GetHandle() == 0) return false;

    m_vertexShader = std::move(vertexShaderSource);
    m_fragmentShader = std::move(fragmentShaderSource);

    ExtractUniform(vertexShaderSource);
    ExtractUniform(fragmentShaderSource);
    return true;
}

bool Shader::CompileFromFile(
    RenderingInterface& renderingInterface,
    const std::string& vertexShaderPath,
    const std::string& fragmentShaderPath)
{
    std::fstream vertexShader, fragmentShader;
    vertexShader.open(vertexShaderPath);
    fragmentShader.open(fragmentShaderPath);

    if (!vertexShader.is_open() || !fragmentShader.is_open())
    {
        LOG_ERROR(LogShader, std::format("Error loading shader VS={} FS={}", vertexShaderPath, fragmentShaderPath));
        return false;
    }

    std::stringstream vertexShaderSource, fragmenShaderSource;

    vertexShaderSource << vertexShader.rdbuf();
    fragmenShaderSource << fragmentShader.rdbuf();

    return CompileFromSource(renderingInterface, vertexShaderSource.str(), fragmenShaderSource.str());
}

bool Shader::Compile(RenderingInterface& renderingInterface)
{
    m_gpuProgram.reset(renderingInterface.CreateGpuProgram());
    m_gpuProgram->BeginCompile()
        .AddSource(m_vertexShader, GpuProgram::Type::Vertex)
        .AddSource(m_fragmentShader, GpuProgram::Type::Fragment)
    .Compile();

    if (m_gpuProgram->GetHandle() == 0) return false;

    ExtractUniform(m_vertexShader);
    ExtractUniform(m_fragmentShader);
    return true;
}

VariantData Shader::Serialize() const
{
    return VariantData::Map {
        {"vertex", m_vertexShaderFile},
        {"fragment", m_fragmentShaderFile}
    };
}

void Shader::Deserialize(const VariantData &data)
{
    m_vertexShaderFile = data["vertex"].GetString();
    m_fragmentShaderFile = data["fragment"].GetString();
}

bool Shader::HasUniformType(Uniform::Type type)
{
    return std::ranges::find_if(m_uniforms, [type](const Uniform& uniform) {
        return uniform.type == type;
    }) != m_uniforms.end();
}

bool Shader::HasUniform(const std::string_view name)
{
    return std::ranges::find_if(m_uniforms, [name](const Uniform& uniform) {
        return uniform.name == name;
    }) != m_uniforms.end();
}

void Shader::ExtractUniform(std::string_view source)
{
    for (const auto line : StrUtil::Split(source, "\n"))
    {
        bool foundUniform = false;
        Uniform uniform {};
        const auto tokens = StrUtil::Split(line, " \t;");

        if (tokens.size() != 3) continue;
        if (tokens[0] != "uniform") continue;

        uniform.name = tokens[2];

        const auto& type = tokens[1];
        if (type == "int") uniform.type = Uniform::Type::Int;
        else if (type == "float") uniform.type = Uniform::Type::Float;
        else if (type == "vec2") uniform.type = Uniform::Type::Vec2;
        else if (type == "vec3") uniform.type = Uniform::Type::Vec3;
        else if (type == "vec4") uniform.type = Uniform::Type::Vec4;
        else if (type == "ivec2") uniform.type = Uniform::Type::Vec2i;
        else if (type == "ivec3") uniform.type = Uniform::Type::Vec3i;
        else if (type == "ivec4") uniform.type = Uniform::Type::Vec4i;
        else if (type == "mat3") uniform.type = Uniform::Type::Mat3;
        else if (type == "mat4") uniform.type = Uniform::Type::Mat4;
        else if (type == "sampler2D") uniform.type = Uniform::Type::Texture2D;
        else { LOG_WARNING(LogShader, std::format("Unknown type: {}", type)); continue; }

        m_uniforms.emplace_back(std::move(uniform));
    }
}