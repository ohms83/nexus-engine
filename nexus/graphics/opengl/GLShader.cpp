//
//  GLShader.cpp
//  Nexus
//
//  Created by nuttachai on 21/10/21.
//

#include "GLShader.hpp"

USING_NAMESPACE_NXS;

GLShader::GLShader()
{
    
}

GLShader::~GLShader()
{
    if (_shaderProgram > 0) {
        glDeleteProgram(_shaderProgram);
    }
}

void GLShader::initWithSource(const char* vertexShaderSource, const char* fragmentShaderSource)
{
    GLuint vertexShader = createGLShader(vertexShaderSource, GL_VERTEX_SHADER);
    GLuint fragmentShader = createGLShader(fragmentShaderSource, GL_FRAGMENT_SHADER);
    
    _shaderProgram = glCreateProgram();
    CHECK_GL_ERROR();
    
    glAttachShader(_shaderProgram, vertexShader);
    CHECK_GL_ERROR();
    glAttachShader(_shaderProgram, fragmentShader);
    CHECK_GL_ERROR();
    glLinkProgram(_shaderProgram);
    CHECK_GL_ERROR();
    
    int  success;
    char infoLog[512];
    glGetProgramiv(_shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(_shaderProgram, 512, NULL, infoLog);
        std::cout << "SHADER PROGRAM CREATION FAILED\n" << infoLog << std::endl;
    }
    
    glDeleteShader(vertexShader);
    CHECK_GL_ERROR();
    glDeleteShader(fragmentShader);
    CHECK_GL_ERROR();
}

GLuint GLShader::createGLShader(const char* source, GLenum type)
{
    GLuint shader = glCreateShader(type);
    CHECK_GL_ERROR();
    glShaderSource(shader, 1, &source, NULL);
    CHECK_GL_ERROR();
    glCompileShader(shader);
    CHECK_GL_ERROR();
    
    int  success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "SHADER COMPILATION FAILED\n" << infoLog << std::endl;
    }
    
    return shader;
}
