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
        
        uint32_t findUniform(const std::string& name) override;
        
        static GLuint createGLShader(const char* source, GLenum type);
        
    protected:
        void setUniformImpl(uint32_t location, Uniform::Type type, float* value) override;
        void updateUniformImpl(uint32_t location, float* value) override;
        
    private:
        GLuint _shaderProgram = 0;
    };
}

#endif /* __NXS_GL_SHADER_H__ */
