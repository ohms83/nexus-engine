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
    std::string&& fragmentShaderSource,
    std::string&& geometryShaderSource)
{
    m_sources[GpuProgram::Type::Vertex] = vertexShaderSource;
    m_sources[GpuProgram::Type::Fragment] = fragmentShaderSource;
    m_sources[GpuProgram::Type::Geometry] = geometryShaderSource;

    m_gpuProgram.reset(renderingInterface.CreateGpuProgram());
    m_gpuProgram->BeginCompile()
        .AddSource(vertexShaderSource, GpuProgram::Type::Vertex)
        .AddSource(fragmentShaderSource, GpuProgram::Type::Fragment)
        .AddSource(geometryShaderSource, GpuProgram::Type::Geometry)
    .Compile();

    if (m_gpuProgram->GetHandle() == 0) return false;

    ExtractUniform(vertexShaderSource);
    ExtractUniform(fragmentShaderSource);
    ExtractUniform(geometryShaderSource);
}

bool Shader::CompileFromSource(
    RenderingInterface& renderingInterface,
    const std::string& vertexShaderSource,
    const std::string& fragmentShaderSource,
    const std::string& geometryShaderSource)
{
    m_sources[GpuProgram::Type::Vertex] = vertexShaderSource;
    m_sources[GpuProgram::Type::Fragment] = fragmentShaderSource;
    m_sources[GpuProgram::Type::Geometry] = geometryShaderSource;

    m_gpuProgram.reset(renderingInterface.CreateGpuProgram());
    m_gpuProgram->BeginCompile()
        .AddSource(vertexShaderSource, GpuProgram::Type::Vertex)
        .AddSource(fragmentShaderSource, GpuProgram::Type::Fragment)
        .AddSource(geometryShaderSource, GpuProgram::Type::Geometry)
    .Compile();

    if (m_gpuProgram->GetHandle() == 0) return false;

    ExtractUniform(vertexShaderSource);
    ExtractUniform(fragmentShaderSource);
    ExtractUniform(geometryShaderSource);
}

bool Shader::CompileFromFile(
    RenderingInterface& renderingInterface,
    const std::string& vertexShaderPath,
    const std::string& fragmentShaderPath,
    const std::string& geometryShaderPath)
{
    std::fstream vertexShader, fragmentShader, geometryShader;
    vertexShader.open(vertexShaderPath);
    fragmentShader.open(fragmentShaderPath);
    geometryShader.open(geometryShaderPath);
    if (!vertexShader.is_open() || !fragmentShader.is_open())
    {
        LOG_ERROR(LogShader, std::format("Error loading shader VS={} FS={}", vertexShaderPath, fragmentShaderPath));
        return false;
    }

    std::stringstream vertexShaderSource, fragmenShaderSource, geometryShaderSource;

    vertexShaderSource << vertexShader.rdbuf();
    fragmenShaderSource << fragmentShader.rdbuf();
    geometryShaderSource << geometryShader.rdbuf();

    return CompileFromSource(renderingInterface, vertexShaderSource.str(), fragmenShaderSource.str(), geometryShaderSource.str());
}

bool Shader::Compile(RenderingInterface& renderingInterface)
{
    const auto vertex = m_sources.at(GpuProgram::Type::Vertex);
    const auto fragment = m_sources.at(GpuProgram::Type::Fragment);
    const auto geometry = m_sources.at(GpuProgram::Type::Geometry);

    m_gpuProgram.reset(renderingInterface.CreateGpuProgram());
    m_gpuProgram->BeginCompile()
        .AddSource(vertex, GpuProgram::Type::Vertex)
        .AddSource(fragment, GpuProgram::Type::Fragment)
        .AddSource(geometry, GpuProgram::Type::Geometry)
    .Compile();

    if (m_gpuProgram->GetHandle() == 0) return false;

    ExtractUniform(vertex);
    ExtractUniform(fragment);
    return true;
}

VariantData Shader::Serialize() const
{
    return VariantData::Map {
        {"vertex", m_sources.at(GpuProgram::Type::Vertex)},
        {"fragment", m_sources.at(GpuProgram::Type::Fragment)},
        {"geometry", m_sources.at(GpuProgram::Type::Geometry)},
    };
}

void Shader::Deserialize(const VariantData &data)
{
    m_sources[GpuProgram::Type::Vertex] = data["vertex"].GetString();
    m_sources[GpuProgram::Type::Fragment] = data["fragment"].GetString();
    m_sources[GpuProgram::Type::Geometry] = data["geometry"].GetString();
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