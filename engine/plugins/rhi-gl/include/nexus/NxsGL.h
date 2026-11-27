#pragma once

#include <glad/glad.h>

#include "nexus/NxsDefine.h"
#include "nexus/debug/LogDispatcher.h"
#include "nexus/graphics/GraphicsConst.h"
#include "nexus/graphics/TextureProxy.h"

#define GL_STRICT_CHECK

DECLARE_LOG_EXTERN(OpenGL);

#ifdef GL_STRICT_CHECK
    #define CHECK_GL_ERROR(func_name) {\
        GLenum error = glGetError();\
        if (error != GL_NO_ERROR) {\
            NXS_ASSERT_MSG(error == GL_NO_ERROR, std::format("Error calling function {0}. Error Code={1:X}", #func_name, error));\
        }\
    }
    #define CHECK_GL_ERROR_MSG(func_name, message) {\
        GLenum error = glGetError();\
        if (error != GL_NO_ERROR) {\
            NXS_ASSERT_MSG(error == GL_NO_ERROR, std::format("Error calling function {0}. Error Code={1:X} Message={2}", #func_name, error, message));\
        }\
    }
#else
    #define CHECK_GL_ERROR(func_name) {\
        GLenum error = glGetError();\
        if (error != GL_NO_ERROR) {\
            Logger::Instance().Log(LogLevel::Error, LogOpenGL, std::format("Error calling function {0}. Error Code={1:X}", #func_name, error);\
        }\
    }
    #define CHECK_GL_ERROR_MSG(func_name, message) {\
        GLenum error = glGetError();\
        if (error != GL_NO_ERROR) {\
            Logger::Instance().Log(LogLevel::Error, LogOpenGL, std::format("Error calling function {0}. Error Code={1:X} Message={2}", #func_name, error, message);\
        }\
    }
#endif
#define CALL_GL_FUNC(func) {\
    func;\
    CHECK_GL_ERROR(#func)\
}
#define CALL_GL_FUNC_MSG(func, message) {\
    func;\
    CHECK_GL_ERROR_MSG(#func, message)\
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

        inline GLenum NxsFacingToGL(const PolygonFacing face)
        {
            switch (face)
            {
            case PolygonFacing::None:
                return GL_NONE;
            case PolygonFacing::Front:
                return GL_FRONT;
            case PolygonFacing::Back:
                return GL_BACK;
            case PolygonFacing::Both:
                return GL_FRONT_AND_BACK;
            }
            NXS_ASSERT(false);
            return GL_NONE;
        }

        inline GLenum NxsPolygonModeToGL(const PolygonMode mode)
        {
            switch (mode)
            {
            case PolygonMode::Fill:
                return GL_FILL;
            case PolygonMode::Wireframe:
                return GL_LINE;
            case PolygonMode::Point:
                return GL_POINT;
            }
            NXS_ASSERT(false);
            return GL_NONE;
        }

        inline bool IsSRGBCapableFormat(PixelFormat format)
        {
            switch (format)
            {
            case PixelFormat::RGB:
            case PixelFormat::RGBA:
            case PixelFormat::Grey:
            case PixelFormat::Red:
            case PixelFormat::RedGreen:
                return true;
            default:
                break;
            }
            return false;
        }

        //! Convert the linear space color format into sRGB
        inline GLint GetSRGBFormat(PixelFormat format)
        {
            GLint gl_internalFormat = GL_NONE;
            switch (format)
            {
            case PixelFormat::RGB:
                gl_internalFormat = GL_SRGB8;
                break;
            case PixelFormat::RGBA:
                gl_internalFormat = GL_SRGB8_ALPHA8;
                break;
            case PixelFormat::Grey:
            case PixelFormat::Red:
                NXS_ASSERT_MSG(GLAD_GL_EXT_texture_sRGB_R8, "sRGB single-channel textures require GL_EXT_texture_sRGB_R8.");
                gl_internalFormat = GL_SR8_EXT;
                break;
            case PixelFormat::RedGreen:
                NXS_ASSERT_MSG(GLAD_GL_EXT_texture_sRGB_RG8, "sRGB two-channel textures require GL_EXT_texture_sRGB_RG8.");
                gl_internalFormat = GL_SRG8_EXT;
                break;
            default:
                NXS_ASSERT_MSG(false, "sRGB color space is only valid for color texture formats.");
                break;
            }
            return gl_internalFormat;
        }

        inline GLint ResolveTextureInternalFormat(
            PixelFormat format,
            TextureColorSpace colorSpace,
            GLint fallbackFormat)
        {
            if (colorSpace == TextureColorSpace::SRgb && IsSRGBCapableFormat(format))
            {
                return GetSRGBFormat(format);
            }

            switch (format)
            {
            case PixelFormat::RGB_16F:
                return GL_RGB16F;
            case PixelFormat::RGBA_16F:
                return GL_RGBA16F;
            default:
                return fallbackFormat;
            }
        }

        inline GLenum GetFrameBufferFormat(FrameBufferType type)
        {
            switch (type)
            {
            case FrameBufferType::Color3:
            case FrameBufferType::Color3F:
                return GL_RGB;
            case FrameBufferType::Color4:
            case FrameBufferType::Color4F:
                return GL_RGBA;
            case FrameBufferType::Depth:
                return GL_DEPTH_COMPONENT;
            case FrameBufferType::Stencil:
                return GL_STENCIL_INDEX;
            case FrameBufferType::DepthStencil:
                return GL_DEPTH24_STENCIL8;
            default:
                NXS_ASSERT_MSG(false, std::format("Invalid framebuffer format {}!!", INT_CAST(type)));
                break;
            }
            return GL_NONE;
        }

        inline GLenum GetFrameBufferInternalFormat(FrameBufferType type)
        {
            switch (type)
            {
            case FrameBufferType::Color3:
            case FrameBufferType::Color4:
                return GL_UNSIGNED_BYTE;
            case FrameBufferType::Color3F:
            case FrameBufferType::Color4F:
            case FrameBufferType::Depth:
                return GL_FLOAT;
            case FrameBufferType::Stencil:
                return GL_INT;
            case FrameBufferType::DepthStencil:
                return GL_UNSIGNED_INT_24_8;
            default:
                NXS_ASSERT_MSG(false, std::format("Invalid framebuffer format {}!!", INT_CAST(type)));
                break;
            }
            return GL_NONE;
        }
    }
}