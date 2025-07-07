//
// Created by nutta on 6/21/2025.
//

#pragma once

#include <nexus/NxsCommon.h>
#include <string>

NXS_NAMESPACE
{
    class Shader
    {
    public:
        enum class Type
        {
            None,
            Vertex,
            Fragment,
            Compute,
            Num
        };

        Shader() = default;
        virtual ~Shader() = default;

        [[nodiscard]] uint32 GetProgramHandle() const
        {
            return m_shaderProgram;
        }

        virtual Shader& BeginCompile();
        virtual Shader& AddSource(const std::string& source, Type shaderType);
        virtual void Compile();

        /**
         * Find the location of a uniform variable with the specified name.
         * A uniform can be considered as shader's global variable where application
         * can use it to control vertex and fragment shader's behavior.
         */
        [[nodiscard]] virtual int32 FindUniform(const std::string& name) const = 0;

        virtual void SetUniformVector(const std::string& name, const glm::vec2& vec) = 0;
        virtual void SetUniformVector(const std::string& name, const glm::vec3& vec) = 0;
        virtual void SetUniformVector(const std::string& name, const glm::vec4& vec) = 0;
        virtual void SetUniformMatrix(const std::string& name, const glm::mat3& matrix, bool tranpose) = 0;
        virtual void SetUniformMatrix(const std::string& name, const glm::mat4& matrix, bool tranpose) = 0;

        virtual void Bind() = 0;
        virtual void Unbind() = 0;

    protected:
        //! Shader program's object ID.
        uint32 m_shaderProgram = 0;
        bool m_compiling = false;
    };
}