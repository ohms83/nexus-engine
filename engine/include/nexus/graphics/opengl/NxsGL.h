#pragma once

#include <glad/glad.h>

#include <iostream>
#include <sstream>
#include <cassert>

#include <nexus/NxsDefine.h>

#define GL_STRICT_CHECK

#ifdef GL_STRICT_CHECK
    #define CHECK_GL_ERROR() {\
        GLenum error = glGetError();\
        if (error != GL_NO_ERROR) {\
            std::stringstream ss;\
            ss << "OPENGL ERROR! ERROR CODE=" << error;\
            std::cout << ss.str() << std::endl;\
            assert(false);\
        }\
    }
#else
    #define CHECK_GL_ERROR() {\
        GLenum error = glGetError();\
        if (error != GL_NO_ERROR) {\
            std::cout << "OPENGL ERROR! ERROR CODE=" << error << std::endl;\
        }\
    }
#endif

NXS_NAMESPACE
{
    namespace GL
    {
        inline GLenum NxsDataToGLenum(const DataType dataType)
        {
            const std::array<GLenum, SIZE_CAST(DataType::Num)> glDataTypes = {
                GL_BYTE,
                GL_SHORT,
                GL_INT,
                GL_INT64_ARB,
                GL_UNSIGNED_BYTE,
                GL_UNSIGNED_SHORT,
                GL_UNSIGNED_INT,
                GL_UNSIGNED_INT64_ARB,
                GL_FLOAT,
                GL_DOUBLE,
            };
            assert(dataType < DataType::Num);
            auto dataTypeIndex = INT_CAST(dataType);
            return glDataTypes[dataTypeIndex];
        }
    }
}