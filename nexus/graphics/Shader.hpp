//
//  Shader.hpp
//  Nexus
//
//  Created by nuttachai on 21/10/21.
//

#ifndef __NXS_SHADER_H__
#define __NXS_SHADER_H__

#include "NxsDefine.h"
#include "io/Data.hpp"

#include <string>
#include <map>

NXS_NAMESPACE {
    class Shader
    {
    public:
        Shader() {}
        virtual ~Shader() {}
        
        const uint32_t getProgramId() const {
            return _shaderProgram;
        }
        
        virtual void initWithSource(const char* vertexShaderSource, const char* fragmentShaderSource) = 0;
        virtual void initWithData(const Data& vertexShaderData, const Data& fragmentShaderData);
        
        /**
         * Find location of a uniform variable with the specified name.
         * A uniform can be considered as shader's global variable where application
         * can use it to control vertex and fragment shader's behaviour.
         */
        virtual uint32_t findUniform(const std::string& name) const = 0;
        
    protected:
        /// Shader program's object ID.
        uint32_t _shaderProgram = 0;
    };
}

#endif /* __NXS_SHADER_H__ */
