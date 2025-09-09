#pragma once

#include <glad/glad.h>

#include <cassert>
#include <format>

#include "nexus/NxsDefine.h"
#include "nexus/core/LogDispatcher.h"
#include "nexus/graphics/GraphicsConst.h"

#define GL_STRICT_CHECK

DECLARE_LOG_EXTERN(OpenGL);

#ifdef GL_STRICT_CHECK
    #define CHECK_GL_ERROR(func_name) {\
        GLenum error = glGetError();\
        NXS_ASSERT_MSG(error == GL_NO_ERROR, std::format("Error calling function {0}. Error Code={1:X}", #func_name, error));\
    }
#else
    #define CHECK_GL_ERROR(func_name) {\
        GLenum error = glGetError();\
        if (error != GL_NO_ERROR) {\
            Logger::Instance().Log(LogLevel::Error, LogOpenGL, std::format("Error calling function {0}. Error Code={1:X}", #func_name, error);\
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

        inline GLuint NxsDrawModeToGL(const DrawMode mode)
        {
            switch (mode)
            {
            case DrawMode::Point:
                return GL_POINTS;
            case DrawMode::Line:
                return GL_LINES;
            case DrawMode::LineStrip:
                return GL_LINE_STRIP;
            case DrawMode::LineLoop:
                return GL_LINE_LOOP;
            case DrawMode::Triangle:
                return GL_TRIANGLES;
            case DrawMode::TriangleStrip:
                return GL_TRIANGLE_STRIP;
            case DrawMode::TriangleFan:
                return GL_TRIANGLE_FAN;
            case DrawMode::Quad:
                return GL_QUADS;
            default:
                NXS_ASSERT(false);
                break;
            }
            return GL_NONE;
        }

        inline GLuint NxsFrontFaceToGL(const FrontFace frontFace)
        {
            switch (frontFace)
            {
            case FrontFace::ClockWise:
                return GL_CW;
            case FrontFace::CounterClockWise:
                return GL_CCW;
            }
            NXS_ASSERT(false);
            return GL_NONE;
        }
    }
}