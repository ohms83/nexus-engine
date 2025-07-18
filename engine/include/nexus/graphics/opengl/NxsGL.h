#pragma once

#include <glad/glad.h>

#include <cassert>
#include <format>

#include <nexus/NxsDefine.h>
#include <nexus/core/Logger.h>

#define GL_STRICT_CHECK

DECLARE_LOG_EXTERN(OpenGL);

#ifdef GL_STRICT_CHECK
    #define CHECK_GL_ERROR(func_name) {\
        GLenum error = glGetError();\
        NXS_ASSERT_MSG(error == GL_NO_ERROR, std::format("Error calling function {0}. Error Code={1}", #func_name, error));\
    }
#else
    #define CHECK_GL_ERROR(func_name) {\
        GLenum error = glGetError();\
        if (error != GL_NO_ERROR) {\
            Logger::Instance().Log(Logger::LogLevel::Fatal, LogOpenGL, std::format("Error calling function {0}. Error Code={1}", func_name, error);\
        }\
    }
#endif
#define CALL_GL_FUNC(func) {\
    func;\
    CHECK_GL_ERROR(#func)\
}

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
                NXS_ASSERT(false);
                return GL_NONE;
            }
        }
    }
}