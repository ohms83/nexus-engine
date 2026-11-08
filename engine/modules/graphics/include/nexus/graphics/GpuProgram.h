#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/debug/Assert.h"

#include "glm/glm.hpp"

#include "GpuResource.h"
#include "TextureProxy.h"

#include <string>
#include <unordered_map>

NXS_NAMESPACE
{
    class GpuProgram
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
        virtual ~GpuProgram() = default;

        NODISCARD uint32_t GetHandle() const
        {
            return m_id;
        }

        virtual GpuProgram& BeginCompile();
        virtual GpuProgram& AddSource(const std::string& source, Type shaderType);
        virtual void Compile();

        virtual void Bind()
        {
            NXS_ASSERT_MSG(false, "Bind not implemented for this graphics API.");
        }

        virtual void Unbind()
        {
            NXS_ASSERT_MSG(false, "Unbind not implemented for this graphics API.");
        }

        virtual bool IsBinding() const
        {
            NXS_ASSERT_MSG(false, "IsBinding not implemented for this graphics API.");
            return false;
        }

        /**
         * Find the location of a uniform variable with the specified name.
         * A uniform can be considered as shader's global variable where application
         * can use it to control vertex and fragment shader's behavior.
         */
        NODISCARD int32_t FindUniform(const std::string& name);

        MAYBE_UNUSED virtual bool SetUniformInt(const std::string& name, int32_t value)
        {
            NXS_ASSERT_MSG(false, "SetUniformInt not implemented for this graphics API.");
            return false;
        }

        MAYBE_UNUSED virtual bool SetUniformFloat(const std::string& name, float value)
        {
            NXS_ASSERT_MSG(false, "SetUniformFloat not implemented for this graphics API.");
            return false;
        }

        MAYBE_UNUSED virtual bool SetUniformVector(const std::string& name, const glm::vec2& vec)
        {
            NXS_ASSERT_MSG(false, "SetUniformVector(vec2) not implemented for this graphics API.");
            return false;
        }

        MAYBE_UNUSED virtual bool SetUniformVector(const std::string& name, const glm::vec3& vec)
        {
            NXS_ASSERT_MSG(false, "SetUniformVector(vec3) not implemented for this graphics API.");
            return false;
        }

        MAYBE_UNUSED virtual bool SetUniformVector(const std::string& name, const glm::vec4& vec)
        {
            NXS_ASSERT_MSG(false, "SetUniformVector(vec4) not implemented for this graphics API.");
            return false;
        }

        MAYBE_UNUSED virtual bool SetUniformMatrix(const std::string& name, const glm::mat3& matrix, bool tranpose)
        {
            NXS_ASSERT_MSG(false, "SetUniformMatrix(mat3) not implemented for this graphics API.");
            return false;
        }

        MAYBE_UNUSED virtual bool SetUniformMatrix(const std::string& name, const glm::mat4& matrix, bool tranpose)
        {
            NXS_ASSERT_MSG(false, "SetUniformMatrix(mat4) not implemented for this graphics API.");
            return false;
        }

        MAYBE_UNUSED virtual bool SetUniformTexture2D(const std::string& name, Ref<const TextureProxy> texture, int32_t textureUnit)
        {
            NXS_ASSERT_MSG(false, "SetUniformTexture2D not implemented for this graphics API.");
            return false;
        }

    protected:
        uint32_t m_id = 0;
        bool m_compiling = false;

        std::unordered_map<std::string, int32_t> m_uniforms;

    private:
        /**
         * Allocating the resource object on GPU. The child classes
         * must provide the implementation of this function.
         * @return The generated resource handle; otherwise 0, if failed.
         */
        NODISCARD virtual uint32_t Alloc()
        {
            NXS_ASSERT_MSG(false, "Alloc not implemented for this graphics API.");
            return 0;
        }

        //! Release the allocated GPU resource.
        virtual void Release()
        {
            NXS_ASSERT_MSG(false, "Release not implemented for this graphics API.");
        }

        NODISCARD virtual int32_t FindUniform_Internal(const std::string& name) const
        {
            NXS_ASSERT_MSG(false, "FindUniform_Internal not implemented for this graphics API.");
            return -1;
        }
    };
}