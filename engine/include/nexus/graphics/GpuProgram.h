#pragma once

#include "nexus/NxsDefine.h"

#include "glm/glm.hpp"

#include "GpuResource.h"
#include "TextureProxy.h"

#include <string>

NXS_NAMESPACE
{
    class GpuProgram : public IGpuResource
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

        GpuProgram() = default;

        NODISCARD uint32 GetHandle() const override
        {
            return m_id;
        }

        virtual GpuProgram& BeginCompile();
        virtual GpuProgram& AddSource(const std::string& source, Type shaderType);
        virtual void Compile();

        virtual bool IsBinding() const = 0;

        /**
         * Find the location of a uniform variable with the specified name.
         * A uniform can be considered as shader's global variable where application
         * can use it to control vertex and fragment shader's behavior.
         */
        NODISCARD virtual int32 FindUniform(const std::string& name) const = 0;

        MAYBE_UNUSED virtual bool SetUniformInt(const std::string& name, int32 value) = 0;
        MAYBE_UNUSED virtual bool SetUniformFloat(const std::string& name, float value) = 0;
        MAYBE_UNUSED virtual bool SetUniformVector(const std::string& name, const glm::vec2& vec) = 0;
        MAYBE_UNUSED virtual bool SetUniformVector(const std::string& name, const glm::vec3& vec) = 0;
        MAYBE_UNUSED virtual bool SetUniformVector(const std::string& name, const glm::vec4& vec) = 0;
        MAYBE_UNUSED virtual bool SetUniformMatrix(const std::string& name, const glm::mat3& matrix, bool tranpose) = 0;
        MAYBE_UNUSED virtual bool SetUniformMatrix(const std::string& name, const glm::mat4& matrix, bool tranpose) = 0;
        MAYBE_UNUSED virtual bool SetUniformTexture2D(const std::string& name, Ref<const TextureProxy> texture, int32 textureUnit) = 0;

    protected:
        uint32 m_id = 0;
        bool m_compiling = false;
    };
}