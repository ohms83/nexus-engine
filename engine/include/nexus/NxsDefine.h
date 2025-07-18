#pragma once

#include <SDL3/SDL.h>
#include <memory>

#if defined(_WIN32)
    #include <windows.h>
    #ifndef NXS_PLATFORM_WINDOWS
#define NXS_PLATFORM_WINDOWS 1
    #endif
#endif

#define NODISCARD [[nodiscard]]

#include <cassert>

#define NXS_NAMESPACE namespace nxs
#define USING_NAMESPACE_NXS using namespace nxs

//! A shorthand definition for static_cast<>
#define CAST static_cast
//! A shorthand definition for dynamic_cast<>
#define D_CAST dynamic_cast
//! A shorthand definition for const_cast<>
#define C_CAST const_cast
//! A shorthand definition for reinterpret_cast<>
#define R_CAST reinterpret_cast

#define SHORT_CAST(x)   CAST<int16_t>(x)
#define INT_CAST(x)     CAST<int32_t>(x)
#define BYTE_CAST(x)    CAST<uint8_t>(x)
#define USHORT_CAST(x)  CAST<uint16_t>(x)
#define UINT_CAST(x)    CAST<uint32_t>(x)
#define SIZE_CAST(x)    CAST<size_t>(x)

NXS_NAMESPACE
{
    typedef int8_t      int8;
    typedef int16_t     int16;
    typedef int32_t     int32;
    typedef int64_t     int64;

    typedef uint8_t     uint8;
    typedef uint16_t    uint16;
    typedef uint32_t    uint32;
    typedef uint64_t    uint64;

    enum class DataType
    {
        None,
        Byte,
        Short,
        Int32,
        Int64,
        UByte,
        UShort,
        UInt32,
        UInt64,
        Float,
        Double,
        Num
    };

    inline size_t NxsDataTypeSize(const DataType type)
    {
        switch (type)
        {
        case DataType::UByte:
        case DataType::Byte:
            return sizeof(uint8);
        case DataType::UShort:
        case DataType::Short:
            return sizeof(uint16);
        case DataType::UInt32:
        case DataType::Int32:
            return sizeof(uint32);
        case DataType::UInt64:
        case DataType::Int64:
            return sizeof(uint64);
        case DataType::Float:
            return sizeof(float);
        case DataType::Double:
            return sizeof(double);
        default:
            assert(0);
            return 0;
        }
    }

    using WindowContext = SDL_Window*;
#ifdef WIN32
    using NativeWindowHandle = HWND;
#else
    static_assert("Unimplemented platform!")
#endif

    union RenderContext
    {
        SDL_GLContext gl_context;
    };

    /**
     * Reference counting type. This is just an alias to @c std::shared_ptr.
     */
    template<typename T>
    using Ref = std::shared_ptr<T>;

    /**
     * Auto-release pointer. This is jus an alias to @c std::unique_ptr.
     */
    template<typename T>
    using Ptr = std::unique_ptr<T>;

    enum class PixelFormat
    {
        None,
        //! 8-bit Red channel
        Red,
        //! 8-bit Green channel
        Green,
        //! 8-bit Blue channel
        Blue,
        //! 8-bit Alpha channel
        Alpha,
        //! 24-bit RGB color without alpha
        RGB,
        //! 32-bit RGBA color.
        RGBA,
        Depth,
        Stencil,

        Num
    };

    template<typename T>
    bool IsA(T* obj)
    {
        return D_CAST<T>(obj) != nullptr;
    }
}