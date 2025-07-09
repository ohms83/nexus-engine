#pragma once

#include <SDL3/SDL.h>

#if defined(_WIN32)
#include <windows.h>

#ifndef NXS_PLATFORM_WINDOWS
#define NXS_PLATFORM_WINDOWS 1
#endif
#endif

#include <array>
#include <cassert>

#define NXS_NAMESPACE namespace nexus
#define USING_NAMESPACE_NXS using namespace nexus

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

    inline size_t NxsDataTypeSize(DataType type)
    {
        static std::array<size_t, SIZE_CAST(DataType::Num)> s_dataSizes = {
            sizeof(uint8),
            sizeof(uint16),
            sizeof(uint32),
            sizeof(uint64),
            sizeof(int8),
            sizeof(int16),
            sizeof(int32),
            sizeof(int64),
            sizeof(float),
            sizeof(double),
        };
        assert(type != DataType::Num);
        return s_dataSizes[INT_CAST(type)];
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
}