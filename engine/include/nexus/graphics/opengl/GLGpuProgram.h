//
// Created by nutta on 7/6/2025.
//

#pragma once

#include <vector>

#include "NxsGL.h"
#include "nexus/graphics/GpuProgram.h"

NXS_NAMESPACE
{
    /**
     * @brief OpenGL implementation of GpuProgram.
     */
    class GLGpuProgram final : public GpuProgram
    {
    public:
        GLGpuProgram() = default;
        ~GLGpuProgram() override;

        GpuProgram& BeginCompile() override;
        GpuProgram& AddSource(const std::string& source, Type GpuProgramType) override;
        void Compile() override;

        bool IsBinding() const override;

        NODISCARD int32 FindUniform(const std::string& name) const override;
        MAYBE_UNUSED bool SetUniformInt(const std::string& name, int32 value) override;
        MAYBE_UNUSED bool SetUniformFloat(const std::string& name, float value) override;
        MAYBE_UNUSED bool SetUniformVector(const std::string& name, const glm::vec2& vec) override;
        MAYBE_UNUSED bool SetUniformVector(const std::string& name, const glm::vec3& vec) override;
        MAYBE_UNUSED bool SetUniformVector(const std::string& name, const glm::vec4& vec) override;
        MAYBE_UNUSED bool SetUniformMatrix(const std::string& name, const glm::mat3& matrix, bool tranpose) override;
        MAYBE_UNUSED bool SetUniformMatrix(const std::string& name, const glm::mat4& matrix, bool tranpose) override;
        MAYBE_UNUSED bool SetUniformTexture2D(const std::string& name, Ref<const TextureProxy> texture, int32 textureUnit) override;

        void Bind() const override;
        void Unbind() const override;

    protected:
        uint32 Alloc() override;
        void Release() override;

        void ClearHandles();
        //! Error GpuProgram is used as a fallback mechanism when the GpuProgram compilation failed.
        // TODO: Remove
        void CreateErrorGpuProgram();

    private:
        //! A list of the handles of vertex, fragment, etc. shaders binding to this gpu program.
        std::vector<GLuint> m_shaderHandles;

        //! The ID of the currently binding gpu program.
        static uint32 s_bindingShader;
        //! For thread safety.
        static std::mutex s_mutex;
    };
}