#pragma once

#include "Shader.h"

#include <unordered_set>
#include <map>

NXS_NAMESPACE
{
    /**
     * @brief A utility class for generating shader source code from the engine's .shader custom format.
     */
    class ShaderGenerator
    {
    public:
        bool GenerateShaderSource(
            const std::string& shaderFilePath,
            std::string& outVertexShader,
            std::string& outFragmentShader,
            std::string& outGeometryShader);

        void SetOutputDirectory(const std::string& outputDir)
        {
            m_outputDirectory = outputDir;
        }

        const std::string& GetOutputDirectory() const
        {
            return m_outputDirectory;
        }

    private:
        bool ParseShaderFile(
            const std::string& shaderFilePath,
            std::map<std::string, std::string>& outSections);

        /**
         * @brief Write the generated shader sources to files. The output files are named
         * based on the shader name and placed in the specified output directory.
         * The default output directory is defined by @c NXS_GENERATED_ASSETS_DIR.
         * 
         * @param shaderFilePath The path to the shader file.
         * @param programName The name of the program being written.
         * @param source The generated shader source code.
         */
        void WriteGeneratedShader(
            const std::string& shaderFilePath,
            const std::string& programName,
            const std::string& source);

        std::string m_outputDirectory = NXS_GENERATED_ASSETS_DIR;

        static std::unordered_set<std::string> s_knownProgramTypes;
        static std::map<std::string, std::string> s_programShortNames;

        static bool IsKnownProgramType(const std::string& name)
        {
            return s_knownProgramTypes.find(name) != s_knownProgramTypes.end();
        }

        static const std::string& GetProgramShortName(const std::string& name);
    };
}