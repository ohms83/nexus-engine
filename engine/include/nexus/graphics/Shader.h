//
// Created by nutta on 6/21/2025.
//

#pragma once

#include "nexus/NxsCommon.h"

#include "nexus/core/Resource.h"
#include "nexus/io/Serializable.h"

#include "GpuProgram.h"
#include "RenderingInterface.h"

#include <string>
#include <string_view>
#include <map>

NXS_NAMESPACE
{
    class Shader : public Resource, public ISerializable
    {
    public:
        struct Uniform
        {
            enum class Type
            {
                Unknown = -1,
                Int,
                Float,
                Vec2i,
                Vec3i,
                Vec4i,
                Vec2,
                Vec3,
                Vec4,
                Mat3,
                Mat4,
                Texture2D,
            };

            std::string name;
            Type type;
        };

        Shader(std::string path, const uint32 id) : Resource(std::move(path), id) {}

        MAYBE_UNUSED bool CompileFromSource(
            const RenderingInterface& renderingInterface,
            std::string&& vertexShaderSource,
            std::string&& fragmentShaderSource,
            std::string&& geometryShaderSource);

        MAYBE_UNUSED bool CompileFromSource(
            const RenderingInterface& renderingInterface,
            const std::string& vertexShaderSource,
            const std::string& fragmentShaderSource,
            const std::string& geometryShaderSource);

        MAYBE_UNUSED bool CompileFromFile(
            const RenderingInterface& renderingInterface,
            const std::string& vertexShaderPath,
            const std::string& fragmentShaderPath,
            const std::string& geometryShaderPath);

        NODISCARD bool HasShader(GpuProgram::Type shaderType) const
        {
            return !GetSource(shaderType).empty();
        }

        const std::string& GetSource(GpuProgram::Type shaderType) const
        {
            if (const auto& itr = m_sources.find(shaderType); itr != m_sources.end())
            {
                return itr->second;
            }
            static const std::string empty = "";
            return empty;
        }

        /**
         * @brief Compile the shader from the existing source files. This is usually called after @c Deserialized.
         * 
         * @param renderingInterface An interface to the underlying rendering API that will perform the compilation.
         * @return true, if success; otherwise, false.
         */
        MAYBE_UNUSED bool Compile(RenderingInterface& renderingInterface);

        NODISCARD Ref<GpuProgram> GetGpuProgram() const
        {
            return m_gpuProgram;
        }

        /**
         * @brief Serializes the shader's current state.
         * @return A @c VariantData object containing the serialized shader.
         */
        VariantData Serialize() const override;

        /**
         * @brief Deserializes the shader from a given @c VariantData.
         * @param data The VariantData container to deserialize from.
         */
        void Deserialize(const VariantData& data) override;

        const std::vector<Uniform>& GetUniforms() const
        {
            return m_uniforms;
        }

        bool HasUniformType(Uniform::Type type);
        bool HasUniform(const std::string_view name);

        /**
         * @brief Parses a uniform type string into a Uniform::Type enum.
         * 
         * @param typeStr The string representation of the uniform type.
         * @return Uniform::Type The parsed uniform type.
         */
        static Uniform::Type ParseUniformType(const std::string& typeStr);
        /**
         * @brief Converts the uniform type enum to its string representation.
         * 
         * @param type The uniform type enum.
         * @return std::string_view The string representation of the uniform type.
         */
        static std::string_view UniformTypeToString(Uniform::Type type);

    private:
        void ExtractUniform(std::string_view source);

        Ref<GpuProgram> m_gpuProgram;
        std::map<GpuProgram::Type, std::string> m_sources;
        std::vector<Uniform> m_uniforms;
    };

    /**
     * @brief A utility class for generating shader source code from the engine's .shader custom format.
     */
    class ShaderGenerator
    {
    public:
        static bool GenerateShaderSource(
            const std::string& shaderFilePath,
            std::string& outVertexShader,
            std::string& outFragmentShader,
            std::string& outGeometryShader);

    private:
        static bool ParseShaderFile(
            const std::string& shaderFilePath,
            std::map<std::string, std::string>& outSections);
    };
}