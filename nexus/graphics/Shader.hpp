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
        /// Uniform is a shader's global variable where application can pass parameter to it.
        struct Uniform
        {
            enum Type
            {
                UNIFORM_VEC2,
                UNIFORM_VEC3,
                UNIFORM_VEC4,
                
                UNIFORM_MAT3,
                UNIFORM_MAT4,
            };
            
            Type type;
            /**
             * Uniform variable's name as being declared in any vertex or fragment shader binding to
             * this shader program.
             */
            std::string name;
            /**
             * Pointer to a variable that is used for passing the value to shader's uniform variable.
             * This variable must valid for the entire shader's life-time.
             */
            float* values;
            
            bool operator < (const Uniform& rhs) const
            {
                return name < rhs.name;
            }
            
            bool operator == (const Uniform& rhs) const
            {
                return (name == rhs.name) && (type == rhs.type);
            }
        };
        
        Shader() {}
        virtual ~Shader() {}
        
        virtual void initWithSource(const char* vertexShaderSource, const char* fragmentShaderSource) = 0;
        virtual void initWithData(const Data& vertexShaderData, const Data& fragmentShaderData);
        
        /**
         * Search and assign a value to the shader's uniform variable as declared in the passing @c uniform parameter
         * @return Uniform's localtion ID as being referred by the shader. This value can be used for re-assigning
         *         new values to the same uniform. 0 will be returned upon errors; eg., the specified uniform name
         *         cannot be found.
         */
        uint32_t setUniform(const Uniform& uniform);
        void updateUniform(uint32_t location, float* value);
        
        virtual uint32_t findUniform(const std::string& name) = 0;
        
    protected:
        virtual void setUniformImpl(uint32_t location, Uniform::Type type, float* value) = 0;
        virtual void updateUniformImpl(uint32_t location, float* value) = 0;
        
    protected:
        std::map<Uniform, uint32_t> _uniforms;
    };
}

#endif /* __NXS_SHADER_H__ */
