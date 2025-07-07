//
// Created by nutta on 7/6/2025.
//

#include <nexus/graphics/opengl/GLShader.h>

#include "glm/gtc/type_ptr.inl"

USING_NAMESPACE_NXS;

static GLenum NxsShaderTypeToGLShader(Shader::Type type)
{
    // Compile shaders
    std::array<GLenum, SIZE_CAST(Shader::Type::Num)> glShaders = {
        GL_NONE,
        GL_VERTEX_SHADER,
        GL_FRAGMENT_SHADER,
        GL_GEOMETRY_SHADER,
    };
    assert(type != Shader::Type::None && type != Shader::Type::Num);
    return glShaders[INT_CAST(type)];
}

// Function to compile a shader
static GLuint CompileShader(const std::string& source, Shader::Type type)
{
    const auto gl_shaderType = NxsShaderTypeToGLShader(type);
    GLuint shader = glCreateShader(gl_shaderType);
    CHECK_GL_ERROR();
    // glShaderSource expects the third parameter to be an immutable pointer to the immutable pointer
    // of the GLchar. Since we can't get a pointer from an R-value returned from c_str(), this is
    // a workaround solution.
    GLchar const* sources[] = { source.c_str() };
    glShaderSource(shader, 1, sources, nullptr);
    CHECK_GL_ERROR();
    glCompileShader(shader);
    CHECK_GL_ERROR();
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    CHECK_GL_ERROR();
    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        CHECK_GL_ERROR();
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    return shader;
}

GLShader::~GLShader()
{
    glDeleteProgram(m_shaderProgram);
}

Shader& GLShader::BeginCompile()
{
    Shader::BeginCompile();
    m_shaderProgram = glCreateProgram();
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

    for (const auto shaderId : m_shaderHandles)
        glAttachShader(m_shaderProgram, shaderId);
    glLinkProgram(m_shaderProgram);
    CHECK_GL_ERROR();
    int success;
    char infoLog[512];
    glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &success);
    CHECK_GL_ERROR();
    if (!success) {
        glGetProgramInfoLog(m_shaderProgram, 512, nullptr, infoLog);
        CHECK_GL_ERROR();
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // We don't need to keep the handles anymore.
    for (const auto shaderId : m_shaderHandles)
        glDeleteShader(shaderId);
    m_shaderHandles.clear();
}

int32 GLShader::FindUniform(const std::string& name) const
{
    return glGetUniformLocation(m_shaderProgram, name.c_str());
}

void GLShader::SetUniformVector(const std::string& name, const glm::vec2& vec)
{
    const auto location = glGetUniformLocation(m_shaderProgram, name.c_str());
    CHECK_GL_ERROR();
    glUniform2fv(location, 1, glm::value_ptr(vec));
    CHECK_GL_ERROR();
}

void GLShader::SetUniformVector(const std::string& name, const glm::vec3& vec)
{
    const auto location = glGetUniformLocation(m_shaderProgram, name.c_str());
    CHECK_GL_ERROR();
    glUniform3fv(location, 1, glm::value_ptr(vec));
    CHECK_GL_ERROR();
}

void GLShader::SetUniformVector(const std::string& name, const glm::vec4& vec)
{
    const auto location = glGetUniformLocation(m_shaderProgram, name.c_str());
    CHECK_GL_ERROR();
    glUniform4fv(location, 1, glm::value_ptr(vec));
    CHECK_GL_ERROR();
}

void GLShader::SetUniformMatrix(const std::string& name, const glm::mat3& matrix, const bool tranpose)
{
    const auto location = glGetUniformLocation(m_shaderProgram, name.c_str());
    CHECK_GL_ERROR();
    glUniformMatrix3fv(location, 1, tranpose ? GL_TRUE : GL_FALSE, glm::value_ptr(matrix));
    CHECK_GL_ERROR();
}

void GLShader::SetUniformMatrix(const std::string& name, const glm::mat4& matrix, const bool tranpose)
{
    const auto location = glGetUniformLocation(m_shaderProgram, name.c_str());
    CHECK_GL_ERROR();
    glUniformMatrix4fv(location, 1, tranpose ? GL_TRUE : GL_FALSE, glm::value_ptr(matrix));
    CHECK_GL_ERROR();
}

void GLShader::Bind()
{
    glUseProgram(m_shaderProgram);
}

void GLShader::Unbind()
{
    glUseProgram(0);
}
