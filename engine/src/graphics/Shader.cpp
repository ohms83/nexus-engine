//
// Created by nutta on 6/21/2025.
//

#include "graphics/Shader.h"
#include "nexus/debug/LogDispatcher.h"
#include "core/StringUtil.h"

#include <fstream>
#include <string_view>
#include <ranges>
#include <filesystem>
#include <set>

USING_NAMESPACE_NXS;

DEFINE_LOG(Shader);

bool Shader::CompileFromSource(
    const RenderingInterface& renderingInterface,
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
    return true;
}

bool Shader::CompileFromSource(
    const RenderingInterface& renderingInterface,
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
    return true;
}

bool Shader::CompileFromFile(
    const RenderingInterface& renderingInterface,
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

bool Shader::Deserialize(const VariantData &data)
{
    m_sources[GpuProgram::Type::Vertex] = data["vertex"].GetString();
    m_sources[GpuProgram::Type::Fragment] = data["fragment"].GetString();
    m_sources[GpuProgram::Type::Geometry] = data["geometry"].GetString();
    return true;
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
    auto lines = StrUtil::Split(source, "\n;");
    for (const auto& line : lines)
    {
        auto copyLine = line;
        // Remove single-line comments
        const auto commentPos = copyLine.find("//");
        if (commentPos != std::string::npos)
        {
            copyLine = copyLine.substr(0, commentPos);
        }

        // Remove inline block comments
        auto startBlockComment = copyLine.find("/*");
        while (startBlockComment != std::string::npos)
        {
            size_t endBlockComment = copyLine.find("*/", startBlockComment);
            // Multi-lines comment is skipped.
            if (endBlockComment == std::string::npos) break;

            // Remove the block comment
            copyLine.replace(startBlockComment, endBlockComment - startBlockComment + 2, "");
            // Find the next block comment
            startBlockComment = copyLine.find("/*");
        }

        auto tokens = StrUtil::Split(StrUtil::Trim(copyLine), ", \t");
        if (tokens.size() < 3) continue;
        if (tokens[0] != "uniform") continue;

        const auto type = ParseUniformType(tokens[1]);
        if (type == Uniform::Type::Unknown) continue;

        // Handle multiple uniform declarations in a single line.
        std::vector<std::string> names(tokens.begin() + 2, tokens.end());
        for (const auto& name : names)
        {
            auto uniformName = name;
            // Remove inline array size if any, e.g., "u_Lights[10]" -> "u_Lights"
            const auto bracketPos = name.find('[');
            if (bracketPos != std::string::npos)
            {
                uniformName = name.substr(0, bracketPos);
            }
            // Remove inline block comments if any, e.g., "u_Color /* color uniform */" -> "u_Color"
            const auto commentPos = uniformName.find("/*");
            if (commentPos != std::string::npos)
            {
                uniformName = uniformName.substr(0, commentPos);
            }

            uniformName = StrUtil::Trim(uniformName);
            if (uniformName.empty()) continue;

            m_uniforms.emplace_back(Shader::Uniform { uniformName, type });
        }
    }
}

Shader::Uniform::Type Shader::ParseUniformType(const std::string &typeStr)
{
    if (typeStr == "int") return Uniform::Type::Int;
    if (typeStr == "float") return Uniform::Type::Float;
    if (typeStr == "vec2") return Uniform::Type::Vec2;
    if (typeStr == "vec3") return Uniform::Type::Vec3;
    if (typeStr == "vec4") return Uniform::Type::Vec4;
    if (typeStr == "ivec2") return Uniform::Type::Vec2i;
    if (typeStr == "ivec3") return Uniform::Type::Vec3i;
    if (typeStr == "ivec4") return Uniform::Type::Vec4i;
    if (typeStr == "mat3") return Uniform::Type::Mat3;
    if (typeStr == "mat4") return Uniform::Type::Mat4;
    if (typeStr == "sampler2D") return Uniform::Type::Texture2D;
    return Uniform::Type::Unknown;
}

std::string_view Shader::UniformTypeToString(Shader::Uniform::Type type)
{
    switch (type)
    {
        case Uniform::Type::Int: return "int";
        case Uniform::Type::Float: return "float";
        case Uniform::Type::Vec2: return "vec2";
        case Uniform::Type::Vec3: return "vec3";
        case Uniform::Type::Vec4: return "vec4";
        case Uniform::Type::Vec2i: return "ivec2";
        case Uniform::Type::Vec3i: return "ivec3";
        case Uniform::Type::Vec4i: return "ivec4";
        case Uniform::Type::Mat3: return "mat3";
        case Uniform::Type::Mat4: return "mat4";
        case Uniform::Type::Texture2D: return "sampler2D";
        default: LOG_WARNING(LogShader, std::format("Unknown uniform type: {}", INT_CAST(type))); return "";
    }
}
