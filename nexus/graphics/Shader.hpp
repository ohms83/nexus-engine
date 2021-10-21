//
//  Shader.hpp
//  Nexus
//
//  Created by nuttachai on 21/10/21.
//

#ifndef __NXS_SHADER_H__
#define __NXS_SHADER_H__

#include "NxsMacros.h"
#include "io/Data.hpp"

#include <string>

NXS_NAMESPACE {
    class Shader
    {
    public:
        Shader() {}
        virtual ~Shader() {}
        
        virtual void initWithSource(const char* vertexShaderSource, const char* fragmentShaderSource) = 0;
        virtual void initWithData(const Data& vertexShaderData, const Data& fragmentShaderData);
    };
}

#endif /* __NXS_SHADER_H__ */
