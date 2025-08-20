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

NXS_NAMESPACE
{
    class Shader : public Resource, public ISerializable
    {
    public:
        struct Uniform
        {
            enum class Type
            {
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

        MAYBE_UNUSED bool CompileFromSource(RenderingInterface& renderingInterface,
            std::string&& vertexShaderSource, std::string&& fragmentShaderSource);
        MAYBE_UNUSED bool CompileFromFile(RenderingInterface& renderingInterface,
            const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
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

    private:

        void ExtractUniform(std::string_view source);

        Ref<GpuProgram> m_gpuProgram;
        std::string m_vertexShaderFile;
        std::string m_fragmentShaderFile;
        std::string m_vertexShader;
        std::string m_fragmentShader;
        std::vector<Uniform> m_uniforms;
    };
}