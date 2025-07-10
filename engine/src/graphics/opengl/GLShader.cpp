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
    glDeleteProgram(m_handle);
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

    for (const auto shaderId : m_shaderHandles)
        glAttachShader(m_handle, shaderId);
    glLinkProgram(m_handle);
    CHECK_GL_ERROR();
    int success;
    char infoLog[512];
    glGetProgramiv(m_handle, GL_LINK_STATUS, &success);
    CHECK_GL_ERROR();
    if (!success) {
        glGetProgramInfoLog(m_handle, 512, nullptr, infoLog);
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
    const auto location = glGetUniformLocation(m_handle, name.c_str());
    CHECK_GL_ERROR();
    return location;
}

void GLShader::SetUniformVector(const std::string& name, const glm::vec2& vec)
{
    const auto location = FindUniform(name);
    glUniform2fv(location, 1, glm::value_ptr(vec));
    CHECK_GL_ERROR();
}

void GLShader::SetUniformVector(const std::string& name, const glm::vec3& vec)
{
    const auto location = FindUniform(name);
    glUniform3fv(location, 1, glm::value_ptr(vec));
    CHECK_GL_ERROR();
}

void GLShader::SetUniformVector(const std::string& name, const glm::vec4& vec)
{
    const auto location = FindUniform(name);
    glUniform4fv(location, 1, glm::value_ptr(vec));
    CHECK_GL_ERROR();
}

void GLShader::SetUniformMatrix(const std::string& name, const glm::mat3& matrix, const bool tranpose)
{
    const auto location = FindUniform(name);
    glUniformMatrix3fv(location, 1, tranpose ? GL_TRUE : GL_FALSE, glm::value_ptr(matrix));
    CHECK_GL_ERROR();
}

void GLShader::SetUniformMatrix(const std::string& name, const glm::mat4& matrix, const bool tranpose)
{
    const auto location = FindUniform(name);
    glUniformMatrix4fv(location, 1, tranpose ? GL_TRUE : GL_FALSE, glm::value_ptr(matrix));
    CHECK_GL_ERROR();
}

void GLShader::SetUniformTexture2D(const std::string& name, const TextureProxy* texture, const int32 textureUnit)
{
    const auto location = FindUniform(name);
    const GLint gl_textureUnit = GL_TEXTURE0 + textureUnit;
    glActiveTexture(gl_textureUnit);
    CHECK_GL_ERROR();
    glBindTexture(GL_TEXTURE_2D, texture->GetHandle());
    CHECK_GL_ERROR();
    glUniform1i(location, textureUnit);
    CHECK_GL_ERROR();
}

void GLShader::Bind() const
{
    glUseProgram(m_handle);
}

void GLShader::Unbind() const
{
    glUseProgram(0);
}

uint32 GLShader::Alloc()
{
    return glCreateProgram();
}

void GLShader::Release()
{
    glDeleteProgram(m_handle);
}
