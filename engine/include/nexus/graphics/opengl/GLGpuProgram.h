//
// Created by nutta on 7/6/2025.
//

#pragma once

#include <vector>

#include "NxsGL.h"
#include "../GpuProgram.h"

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

        void Bind() override;
        void Unbind() override;
        bool IsBinding() const override;

        MAYBE_UNUSED bool SetUniformInt(const std::string& name, int32_t value) override;
        MAYBE_UNUSED bool SetUniformFloat(const std::string& name, float value) override;
        MAYBE_UNUSED bool SetUniformVector(const std::string& name, const glm::vec2& vec) override;
        MAYBE_UNUSED bool SetUniformVector(const std::string& name, const glm::vec3& vec) override;
        MAYBE_UNUSED bool SetUniformVector(const std::string& name, const glm::vec4& vec) override;
        MAYBE_UNUSED bool SetUniformMatrix(const std::string& name, const glm::mat3& matrix, bool tranpose) override;
        MAYBE_UNUSED bool SetUniformMatrix(const std::string& name, const glm::mat4& matrix, bool tranpose) override;
        MAYBE_UNUSED bool SetUniformTexture2D(const std::string& name, Ref<const TextureProxy> texture, int32_t textureUnit) override;

    protected:
        void ClearHandles();
        //! Error GpuProgram is used as a fallback mechanism when the GpuProgram compilation failed.
        // TODO: Remove
        void CreateErrorGpuProgram();

    private:
        uint32_t Alloc() override;
        void Release() override;
        NODISCARD int32_t FindUniform_Internal(const std::string& name) const override;

        //! A list of the handles of vertex, fragment, etc. shaders binding to this gpu program.
        std::vector<GLuint> m_shaderHandles;

        //! The ID of the currently binding gpu program.
        static std::atomic<uint32_t> s_bindingShader;
    };
}