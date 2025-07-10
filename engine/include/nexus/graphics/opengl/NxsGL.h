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
            switch (dataType)
            {
            case DataType::Byte:
                return GL_BYTE;
            case DataType::Short:
                return GL_SHORT;
            case DataType::Int32:
                return GL_INT;
            case DataType::Int64:
                return GL_INT64_ARB;
            case DataType::UByte:
                return GL_UNSIGNED_BYTE;
            case DataType::UShort:
                return GL_UNSIGNED_SHORT;
            case DataType::UInt32:
                return GL_UNSIGNED_INT;
            case DataType::UInt64:
                return GL_UNSIGNED_INT64_ARB;
            case DataType::Float:
                return GL_FLOAT;
            case DataType::Double:
                return GL_DOUBLE;
            default:
                assert(false);
                return GL_NONE;
            }
        }
    }
}