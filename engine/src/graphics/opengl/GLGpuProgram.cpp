//
// Created by nutta on 7/6/2025.
//

#include "graphics/opengl/GLGpuProgram.h"
#include "glm/gtc/type_ptr.inl"

// GpuProgram sources
const char* error_vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 _Model;
uniform mat4 _View;
uniform mat4 _Projection;

void main()
{
    gl_Position = _Projection * _View * _Model * vec4(aPos, 1.0);
}
)";

const char* error_fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

void main()
{
    // Showing the magenta color to imply the error.
    FragColor = vec4(1, 0, 1, 1.0);
}
)";

#define CHECK_IF_BINDING(return_value) if (!IsBinding()) { \
    LOG_FATAL(LogOpenGL, "Invalid operation. Cannot set a uniform parameter without binding."); \
    return return_value; \
}

NXS_NAMESPACE
{
    namespace GL
    {
        static GLenum NxsShaderTypeToGLShader(GpuProgram::Type type)
        {
            switch (type)
            {
            case GpuProgram::Type::Vertex:
                return GL_VERTEX_SHADER;
            case GpuProgram::Type::Fragment:
                return GL_FRAGMENT_SHADER;
            case GpuProgram::Type::Geometry:
                return GL_GEOMETRY_SHADER;
            default:
                assert(false);
                return GL_NONE;
            }
        }
    }
}

USING_NAMESPACE_NXS;

uint32 GLGpuProgram::s_bindingShader = 0;
//! For thread safety.
std::mutex GLGpuProgram::s_mutex;

// Function to compile a shader
static GLuint CompileShader(const std::string& source, GpuProgram::Type type)
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

GLGpuProgram::~GLGpuProgram()
{
    CALL_GL_FUNC(glDeleteProgram(m_id));
}

GpuProgram& GLGpuProgram::BeginCompile()
{
    GpuProgram::BeginCompile();
    m_id = Alloc();
    return *this;
}

GpuProgram& GLGpuProgram::AddSource(const std::string& source, Type shaderType)
{
    GpuProgram::AddSource(source, shaderType);
    if (const auto shaderId = CompileShader(source, shaderType); shaderId != 0) m_shaderHandles.push_back(shaderId);
    return *this;
}

void GLGpuProgram::Compile()
{
    GpuProgram::Compile();

    for (const auto shaderId : m_shaderHandles) {
        CALL_GL_FUNC(glAttachShader(m_id, shaderId));
    }
    CALL_GL_FUNC(glLinkProgram(m_id));
    int success;
    char infoLog[512];
    CALL_GL_FUNC(glGetProgramiv(m_id, GL_LINK_STATUS, &success));
    if (!success)
    {
        CALL_GL_FUNC(glGetProgramInfoLog(m_id, 512, nullptr, infoLog));
        LOG_ERROR(LogOpenGL, std::format("ERROR LINKING SHADER PROGRAM\nErrorLogs={}", infoLog));

        // Recreate shaders using the error shader source.
        CALL_GL_FUNC(glDeleteProgram(m_id));
        ClearHandles();
        CreateErrorGpuProgram();
    }

    // We don't need to keep the handles anymore.
    ClearHandles();
}

bool GLGpuProgram::IsBinding() const
{
    return GetHandle() == s_bindingShader;
}

int32 GLGpuProgram::FindUniform(const std::string& name) const
{
    const auto location = glGetUniformLocation(m_id, name.c_str());
    CHECK_GL_ERROR(glGetUniformLocation);
    return location;
}

bool GLGpuProgram::SetUniformInt(const std::string& name, const int32 value)
{
    CHECK_IF_BINDING(false);
    const auto location = FindUniform(name);
    if (location < 0) return false;
    CALL_GL_FUNC(glUniform1i(location, value));
    return true;
}

bool GLGpuProgram::SetUniformFloat(const std::string& name, const float value)
{
    CHECK_IF_BINDING(false);
    const auto location = FindUniform(name);
    if (location < 0) return false;
    CALL_GL_FUNC(glUniform1f(location, value));
    return true;
}

bool GLGpuProgram::SetUniformVector(const std::string& name, const glm::vec2& vec)
{
    CHECK_IF_BINDING(false);
    const auto location = FindUniform(name);
    if (location < 0) return false;
    CALL_GL_FUNC(glUniform2fv(location, 1, glm::value_ptr(vec)));
    return true;
}

bool GLGpuProgram::SetUniformVector(const std::string& name, const glm::vec3& vec)
{
    CHECK_IF_BINDING(false);
    const auto location = FindUniform(name);
    if (location < 0) return false;
    CALL_GL_FUNC(glUniform3fv(location, 1, glm::value_ptr(vec)));
    return true;
}

bool GLGpuProgram::SetUniformVector(const std::string& name, const glm::vec4& vec)
{
    CHECK_IF_BINDING(false);
    const auto location = FindUniform(name);
    if (location < 0) return false;
    CALL_GL_FUNC(glUniform4fv(location, 1, glm::value_ptr(vec)));
    return true;
}

bool GLGpuProgram::SetUniformMatrix(const std::string& name, const glm::mat3& matrix, const bool tranpose)
{
    CHECK_IF_BINDING(false);
    const auto location = FindUniform(name);
    if (location < 0) return false;
    CALL_GL_FUNC(glUniformMatrix3fv(location, 1, tranpose ? GL_TRUE : GL_FALSE, glm::value_ptr(matrix)));
    return true;
}

bool GLGpuProgram::SetUniformMatrix(const std::string& name, const glm::mat4& matrix, const bool tranpose)
{
    CHECK_IF_BINDING(false);
    const auto location = FindUniform(name);
    if (location < 0) return false;
    CALL_GL_FUNC(glUniformMatrix4fv(location, 1, tranpose ? GL_TRUE : GL_FALSE, glm::value_ptr(matrix)));
    return true;
}

bool GLGpuProgram::SetUniformTexture2D(const std::string& name, Ref<const TextureProxy> texture, const int32 textureUnit)
{
    CHECK_IF_BINDING(false);
    const auto location = FindUniform(name);
    if (location < 0) return false;
    const GLint gl_textureUnit = GL_TEXTURE0 + textureUnit;
    CALL_GL_FUNC(glActiveTexture(gl_textureUnit));
    CALL_GL_FUNC(glBindTexture(GL_TEXTURE_2D, texture->GetHandle()));
    CALL_GL_FUNC(glUniform1i(location, textureUnit));
    return true;
}

void GLGpuProgram::Bind() const
{
    std::lock_guard<std::mutex> lock(s_mutex);
    CALL_GL_FUNC(glUseProgram(m_id));
    s_bindingShader = GetHandle();
}

void GLGpuProgram::Unbind() const
{
    CALL_GL_FUNC(glUseProgram(0));
}

uint32 GLGpuProgram::Alloc()
{
    const auto programHandle = glCreateProgram();
    CHECK_GL_ERROR(glCreateProgram);
    return programHandle;
}

void GLGpuProgram::Release()
{
    CALL_GL_FUNC(glDeleteProgram(m_id));
}

void GLGpuProgram::ClearHandles()
{
    for (const auto shaderId : m_shaderHandles) {
        CALL_GL_FUNC(glDeleteShader(shaderId));
    }
    m_shaderHandles.clear();
}

void GLGpuProgram::CreateErrorGpuProgram()
{
    GLint success = 1;

    m_id = Alloc();
    m_shaderHandles.push_back(CompileShader(error_vertexShaderSource, Type::Vertex));
    m_shaderHandles.push_back(CompileShader(error_fragmentShaderSource, Type::Fragment));

    for (const auto shaderId : m_shaderHandles) {
        CALL_GL_FUNC(glAttachShader(m_id, shaderId));
    }
    CALL_GL_FUNC(glLinkProgram(m_id));
    CALL_GL_FUNC(glGetProgramiv(m_id, GL_LINK_STATUS, &success));
    if (!success) {
        // Something went horribly wrong
        LOG_FATAL(LogOpenGL, std::format("Failed to create the error shader!"));
    }
}
