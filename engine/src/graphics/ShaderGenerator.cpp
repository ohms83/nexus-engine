#include "graphics/ShaderGenerator.h"
#include "core/LogDispatcher.h"
#include "core/StringUtil.h"

#include <fstream>
#include <filesystem>
#include <set>
#include <format>

USING_NAMESPACE_NXS;

DEFINE_LOG(ShaderGenerator);

static bool ProcessIncludes(std::stringstream& stream, const std::filesystem::path& currentDir, std::set<std::filesystem::path>& includedFiles, std::ostringstream& outSourceStream)
{
    std::string line;

    while (std::getline(stream, line))
    {
        if (StrUtil::Trim(line).rfind("@include", 0) == 0)
        {
            auto includeSubstr = StrUtil::Trim(line.substr(8)); // Get what's after @include
            auto pathStart = includeSubstr.find('\"');
            auto pathEnd = includeSubstr.rfind('\"');

            if (pathStart == std::string::npos || pathEnd == std::string::npos || pathStart == pathEnd)
            {
                LOG_WARNING(LogShaderGenerator, std::format("Malformed include directive: {}", line));
                outSourceStream << line << '\n';
                continue;
            }

            auto includePathStr = includeSubstr.substr(pathStart + 1, pathEnd - pathStart - 1);
            auto includeFilePath = std::filesystem::absolute(currentDir / includePathStr);

            if (includedFiles.count(includeFilePath))
            {
                continue; // Skip already included file
            }

            std::fstream includeFile(includeFilePath);
            if (!includeFile.is_open())
            {
                LOG_ERROR(LogShaderGenerator, std::format("Cannot open include file: {}. Base directory: {}", includeFilePath.string(), currentDir.string()));
                outSourceStream << line << '\n'; // Keep the problematic include for debugging
                return false;
            }

            includedFiles.insert(includeFilePath);

            std::stringstream includeSourceStream;
            includeSourceStream << includeFile.rdbuf();
            includeFile.close();

            if (!ProcessIncludes(includeSourceStream, includeFilePath.parent_path(), includedFiles, outSourceStream)) {
                return false;
            }
        }
        else
        {
            outSourceStream << line << '\n';
        }
    }
    return true;
}

std::unordered_set<std::string> ShaderGenerator::s_knownProgramTypes {
    "vertex",
    "fragment",
    "geometry",
};

std::map<std::string, std::string> ShaderGenerator::s_programShortNames {
    {"vertex", "vert"},
    {"fragment", "frag"},
    {"geometry", "geom"},
};

// These extensions work better with the syntax-highlighters.
std::map<std::string, std::string> ShaderGenerator::s_programExtensions {
    {"vertex", "vert"},
    {"fragment", "frag"},
    {"geometry", "geom"},
};

bool ShaderGenerator::GenerateShaderSource(const std::string &shaderFilePath, std::string &outVertexShader, std::string &outFragmentShader, std::string &outGeometryShader)
{
    auto shaderSources = std::map<std::string, std::string> {
        {"vertex", ""},
        {"fragment", ""},
        {"geometry", ""}
    };
    bool success = ParseShaderFile(shaderFilePath, shaderSources);

    outVertexShader = std::move(shaderSources["vertex"]);
    outFragmentShader = std::move(shaderSources["fragment"]);
    outGeometryShader = std::move(shaderSources["geometry"]);

    return success;
}

bool ShaderGenerator::ParseShaderFile(const std::string& shaderFilePath, std::map<std::string, std::string>& outPrograms)
{
    std::fstream shaderFile(shaderFilePath);
    if (!shaderFile.is_open())
    {
        LOG_ERROR(LogShaderGenerator, std::format("Error loading shader file: {}", shaderFilePath));
        return false;
    }

    std::filesystem::path fsPath(shaderFilePath);
    auto shaderDir = fsPath.parent_path();

    std::string line;
    std::string glslVersion;

    while (std::getline(shaderFile, line))
    {
        line = StrUtil::Trim(line);
        if (line.rfind("@glsl_version", 0) == 0)
        {
            glslVersion = StrUtil::Trim(line.substr(13));
        }
        else if (line.rfind("@program", 0) == 0)
        {
            if (glslVersion.empty())
            {
                LOG_ERROR(LogShaderGenerator, std::format("GLSL version not specified before programs in shader file: {}", shaderFilePath));
                return false;
            }

            const auto programName = StrUtil::Trim(line.substr(8));
            std::stringstream programSource;
            if (!IsKnownProgramType(programName))
            {
                LOG_ERROR(LogShaderGenerator, std::format("Unknown program type: {}", programName));
                return false;
            }

            // Prepend GLSL version
            programSource << "#version " << glslVersion << "\n\n";
            // Read until @endprogram
            while (std::getline(shaderFile, line))
            {
                if (StrUtil::Trim(line).rfind("@endprogram", 0) == 0)
                {
                    break;
                }
                programSource << line << "\n";
            }

            std::set<std::filesystem::path> includedFiles;
            includedFiles.insert(std::filesystem::absolute(fsPath)); // Add the root file itself

            // Process all @include statements
            std::ostringstream outSourceStream;
            if (!ProcessIncludes(programSource, shaderDir, includedFiles, outSourceStream)) {
                return false;
            }

            // Write to file for debugging/inspection.
            const auto source = std::move(outSourceStream.str());
            WriteGeneratedShader(shaderFilePath, programName, source);
            outPrograms[programName] = std::move(source);
        }
    }

    if (outPrograms.empty())
    {
        LOG_ERROR(LogShaderGenerator, std::format("No programs found in shader file: {}", shaderFilePath));
        return false;
    }

    return true;
}

void ShaderGenerator::WriteGeneratedShader(const std::string& shaderFilePath, const std::string& programName, const std::string& source)
{
    if (m_outputDirectory.empty()) return;

    try {
        // Ensure the output directory exists
        std::filesystem::path outDirPath(m_outputDirectory);
        if (!std::filesystem::exists(outDirPath)) {
            std::filesystem::create_directories(outDirPath);
        }

        // Generate the filename: e.g., "default.vert"
        std::filesystem::path sourcePath(shaderFilePath);
        const auto& extension = s_programExtensions.at(programName);
        const auto fileName = std::format("{}.{}", sourcePath.stem().string(), extension);
        std::filesystem::path fullPath = outDirPath / fileName;

        // Write the file
        std::ofstream outFile(fullPath, std::ios::out | std::ios::trunc);
        if (!outFile.is_open())
        {
            // Writing generated shader is optional and should be treated as an ignorable error.
            LOG_WARNING(LogShaderGenerator, std::format("Failed to open file for writing: {}", fullPath.string()));
            return;
        }

        outFile << source;
        outFile.close();
        
        LOG_INFO(LogShaderGenerator, std::format("Generated shader written to: {}", fullPath.string()));
    }
    catch (const std::exception& e) {
        LOG_ERROR(LogShaderGenerator, std::format("Error while writing generated shader. Program Name: {}. Error: {}", programName, e.what()));
    }
}
const std::string &ShaderGenerator::GetProgramShortName(const std::string &name)
{
    try {
        return s_programShortNames.at(name);
    }
    catch(const std::out_of_range& ex) {
        LOG_ERROR(LogShaderGenerator, std::format("Invalid program name: {}", name));
        static const std::string empty;
        return empty;
    }
}