//
// Created by nutta on 7/6/2025.
//

#include <nexus/graphics/opengl/GLShader.h>

#include "glm/gtc/type_ptr.inl"

NXS_NAMESPACE
{
    namespace GL
    {
        static GLenum NxsShaderTypeToGLShader(Shader::Type type)
        {
            switch (type)
            {
            case Shader::Type::Vertex:
                return GL_VERTEX_SHADER;
            case Shader::Type::Fragment:
                return GL_FRAGMENT_SHADER;
            case Shader::Type::Geometry:
                return GL_GEOMETRY_SHADER;
            default:
                assert(false);
                return GL_NONE;
            }
        }
    }
}

USING_NAMESPACE_NXS;

// Function to compile a shader
static GLuint CompileShader(const std::string& source, Shader::Type type)
{
    const auto gl_shaderType = GL::NxsShaderTypeToGLShader(type);
    const GLuint shader = glCreateShader(gl_shaderType);
    CHECK_GL_ERROR(glCreateShader);
    // glShaderSource expects the third parameter to be an immutable pointer to the immutable pointer
    // of the GLchar. Since we can't get a pointer from an R-value returned from c_str(), this is
    // a workaround solution.
    GLchar const* sources[] = { source.c_str() };
    CALL_GL_FUNC(glShaderSource(shader, 1, sources, nullptr));
    CALL_GL_FUNC(glCompileShader(shader));
    int success;
    char infoLog[512];
    CALL_GL_FUNC(glGetShaderiv(shader, GL_COMPILE_STATUS, &success));
    if (!success) {
        CALL_GL_FUNC(glGetShaderInfoLog(shader, 512, nullptr, infoLog));
        LOG_ERROR(LogOpenGL, std::format("SHADER COMPILATION FAILED\nErrorLogs={}", infoLog));
    }
    return shader;
}

GLShader::~GLShader()
{
    CALL_GL_FUNC(glDeleteProgram(m_handle));
}

Shader& GLShader::BeginCompile()
{
    Shader::BeginCompile();
    m_handle = Alloc();
    return *this;
}

Shader& GLShader::AddSource(const std::string& source, Type shaderType)
{
    Shader::AddSource(source, shaderType);
    if (const auto shaderId = CompileShader(source, shaderType); shaderId != 0) m_shaderHandles.push_back(shaderId);
    return *this;
}

void GLShader::Compile()
{
    Shader::Compile();

    for (const auto shaderId : m_shaderHandles) {
        CALL_GL_FUNC(glAttachShader(m_handle, shaderId));
    }
    CALL_GL_FUNC(glLinkProgram(m_handle));
    int success;
    char infoLog[512];
    CALL_GL_FUNC(glGetProgramiv(m_handle, GL_LINK_STATUS, &success));
    if (!success) {
        CALL_GL_FUNC(glGetProgramInfoLog(m_handle, 512, nullptr, infoLog));
        LOG_ERROR(LogOpenGL, std::format("ERROR LINKING SHADER PROGRAM\nErrorLogs={}", infoLog));
    }

    // We don't need to keep the handles anymore.
    for (const auto shaderId : m_shaderHandles) {
        CALL_GL_FUNC(glDeleteShader(shaderId));
    }
    m_shaderHandles.clear();
}

int32 GLShader::FindUniform(const std::string& name) const
{
    const auto location = glGetUniformLocation(m_handle, name.c_str());
    CHECK_GL_ERROR(glGetUniformLocation);
    return location;
}

void GLShader::SetUniformInt(const std::string& name, const int32 value)
{
    const auto location = FindUniform(name);
    CALL_GL_FUNC(glUniform1i(location, value));
}

void GLShader::SetUniformFloat(const std::string& name, const float value)
{
    const auto location = FindUniform(name);
    CALL_GL_FUNC(glUniform1f(location, value));
}

void GLShader::SetUniformVector(const std::string& name, const glm::vec2& vec)
{
    const auto location = FindUniform(name);
    CALL_GL_FUNC(glUniform2fv(location, 1, glm::value_ptr(vec)));
}

void GLShader::SetUniformVector(const std::string& name, const glm::vec3& vec)
{
    const auto location = FindUniform(name);
    CALL_GL_FUNC(glUniform3fv(location, 1, glm::value_ptr(vec)));
}

void GLShader::SetUniformVector(const std::string& name, const glm::vec4& vec)
{
    const auto location = FindUniform(name);
    CALL_GL_FUNC(glUniform4fv(location, 1, glm::value_ptr(vec)));
}

void GLShader::SetUniformMatrix(const std::string& name, const glm::mat3& matrix, const bool tranpose)
{
    const auto location = FindUniform(name);
    CALL_GL_FUNC(glUniformMatrix3fv(location, 1, tranpose ? GL_TRUE : GL_FALSE, glm::value_ptr(matrix)));
}

void GLShader::SetUniformMatrix(const std::string& name, const glm::mat4& matrix, const bool tranpose)
{
    const auto location = FindUniform(name);
    CALL_GL_FUNC(glUniformMatrix4fv(location, 1, tranpose ? GL_TRUE : GL_FALSE, glm::value_ptr(matrix)));
}

void GLShader::SetUniformTexture2D(const std::string& name, Ref<const TextureProxy> texture, const int32 textureUnit)
{
    const auto location = FindUniform(name);
    const GLint gl_textureUnit = GL_TEXTURE0 + textureUnit;
    CALL_GL_FUNC(glActiveTexture(gl_textureUnit));
    CALL_GL_FUNC(glBindTexture(GL_TEXTURE_2D, texture->GetHandle()));
    CALL_GL_FUNC(glUniform1i(location, textureUnit));
}

void GLShader::Bind() const
{
    CALL_GL_FUNC(glUseProgram(m_handle));
}

void GLShader::Unbind() const
{
    CALL_GL_FUNC(glUseProgram(0));
}

uint32 GLShader::Alloc()
{
    const auto programHandle = glCreateProgram();
    CHECK_GL_ERROR(glCreateProgram);
    return programHandle;
}

void GLShader::Release()
{
    CALL_GL_FUNC(glDeleteProgram(m_handle));
}
