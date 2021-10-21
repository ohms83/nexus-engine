//
//  GLShader.hpp
//  Nexus
//
//  Created by nuttachai on 21/10/21.
//

#ifndef __NXS_GL_SHADER_H__
#define __NXS_GL_SHADER_H__

#include "graphics/Shader.hpp"
#include "NxsGL.h"

NXS_NAMESPACE {
    class GLShader : public Shader
    {
    public:
        GLShader();
        virtual ~GLShader();
        
        void initWithSource(const char* vertexShaderSource, const char* fragmentShaderSource) override;
        
        const GLuint getShaderProgram() const {
            return _shaderProgram;
        }
        
        static GLuint createGLShader(const char* source, GLenum type);
        
    private:
        GLuint _shaderProgram = 0;
    };
}

#endif /* __NXS_GL_SHADER_H__ */
