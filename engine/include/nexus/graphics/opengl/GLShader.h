//
// Created by nutta on 7/6/2025.
//

#pragma once

#include <vector>

#include "NxsGL.h"
#include "nexus/graphics/Shader.h"

NXS_NAMESPACE
{
    class GLShader final : public Shader
    {
    public:
        GLShader() = default;
        ~GLShader() override;

        Shader& BeginCompile() override;
        Shader& AddSource(const std::string& source, Type shaderType) override;
        void Compile() override;

        NODISCARD int32 FindUniform(const std::string& name) const override;
        void SetUniformVector(const std::string& name, const glm::vec2& vec) override;
        void SetUniformVector(const std::string& name, const glm::vec3& vec) override;
        void SetUniformVector(const std::string& name, const glm::vec4& vec) override;
        void SetUniformMatrix(const std::string& name, const glm::mat3& matrix, bool tranpose) override;
        void SetUniformMatrix(const std::string& name, const glm::mat4& matrix, bool tranpose) override;

        void SetUniformTexture2D(const std::string& name, const TextureProxy* texture, int32 textureUnit) override;

        void Bind() const override;
        void Unbind() const override;

    protected:
        uint32 Alloc() override;
        void Release() override;

    private:
        std::vector<GLuint> m_shaderHandles;
    };
}