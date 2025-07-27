//
// Created by nutta on 6/21/2025.
//

#pragma once

#include <nexus/NxsCommon.h>
#include <string>
#include <unordered_map>

#include "GpuResrouce.h"
#include "TextureProxy.h"

NXS_NAMESPACE
{
    class Shader : public GpuResrouce
    {
    public:
        enum class Type
        {
            None,
            Vertex,
            Fragment,
            Geometry,
            Num
        };

        Shader() = default;

        virtual Shader& BeginCompile();
        virtual Shader& AddSource(const std::string& source, Type shaderType);
        virtual void Compile();

        /**
         * Find the location of a uniform variable with the specified name.
         * A uniform can be considered as shader's global variable where application
         * can use it to control vertex and fragment shader's behavior.
         */
        NODISCARD virtual int32 FindUniform(const std::string& name) const = 0;

        virtual void SetUniformFloat(const std::string& name, float value) = 0;
        virtual void SetUniformVector(const std::string& name, const glm::vec2& vec) = 0;
        virtual void SetUniformVector(const std::string& name, const glm::vec3& vec) = 0;
        virtual void SetUniformVector(const std::string& name, const glm::vec4& vec) = 0;
        virtual void SetUniformMatrix(const std::string& name, const glm::mat3& matrix, bool tranpose) = 0;
        virtual void SetUniformMatrix(const std::string& name, const glm::mat4& matrix, bool tranpose) = 0;

        virtual void SetUniformTexture2D(const std::string& name, const TextureProxy* texture, int32 textureUnit) = 0;

    protected:
        bool m_compiling = false;
    };
}