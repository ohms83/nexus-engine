#pragma once

#include <SDL3/SDL.h>
#include <memory>
#include <cstring>

#include "NxsDataType.h"

#ifdef NXS_BUILD_DEBUG
constexpr char const* NXS_BUILD_CFG = "Debug";
#elif defined(NXS_BUILD_RELWITHDEBINFO)
constexpr char const* NXS_BUILD_CFG = "RelWithDebInfo";
#elif defined(NXS_BUILD_RELEASE)
constexpr char const* NXS_BUILD_CFG = "Release";
#else
constexpr char const* NXS_BUILD_CFG = "Unknown";
#endif

#if defined(_WIN32)
    #ifndef NXS_PLATFORM_WINDOWS
        #define NXS_PLATFORM_WINDOWS 1
    #endif
#endif

#define NODISCARD [[nodiscard]]
#define MAYBE_UNUSED [[maybe_unused]]

#include <functional>

#define NXS_NAMESPACE namespace nxs
#define USING_NAMESPACE_NXS using namespace nxs

#define NXS_INHERIT(Parent) private: using Super = Parent;

//! A shorthand definition for static_cast<>
#define CAST static_cast
//! A shorthand definition for dynamic_cast<>
#define D_CAST dynamic_cast
//! A shorthand definition for const_cast<>
#define C_CAST const_cast
//! A shorthand definition for reinterpret_cast<>
#define R_CAST reinterpret_cast
#define PTR_CAST std::dynamic_pointer_cast

#define SHORT_CAST(x)   CAST<int16_t>(x)
#define INT_CAST(x)     CAST<int32_t>(x)
#define INT64_CAST(x)   CAST<int64_t>(x)
#define BYTE_CAST(x)    CAST<uint8_t>(x)
#define USHORT_CAST(x)  CAST<uint16_t>(x)
#define UINT_CAST(x)    CAST<uint32_t>(x)
#define UINT64_CAST(x)  CAST<uint64_t>(x)
#define FLOAT_CAST(x)   CAST<float>(x)
#define DOUBLE_CAST(x)  CAST<double>(x)
#define SIZE_CAST(x)    CAST<size_t>(x)

#define SINGLETON(Type) static Type& Instance() { static Type instance; return instance; }

#define DECLARE_LOG_EXTERN(LogCategory) extern const std::string Log##LogCategory
#define DEFINE_LOG(LogCategory) const std::string Log##LogCategory = #LogCategory

#ifdef NXS_PLATFORM_WIN64
#define NXS_STRNCPY(dst, dstsize, src, count) strncpy_s(dst, dstsize, src, count)
#else
#define NXS_STRNCPY(dst, dstsize, src, count) strncpy(dst, src, count)
#endif

NXS_NAMESPACE
{
    using WindowContext = SDL_Window*;

    using Identifier = uint32_t;
    const extern Identifier InvalidID;
    const extern Identifier MaxIdentifier;

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
     * Auto-release pointer. This is just an alias to @c std::unique_ptr.
     */
    template<typename T>
    using Ptr = std::unique_ptr<T>;

    enum class PixelFormat
    {
        None,
        //! Single channel grayscale.
        Grey,
        //! 8-bit Red channel
        Red,
        //! 8-bit Green channel
        Green,
        //! 8-bit Blue channel
        Blue,
        //! 8-bit Alpha channel
        Alpha,
        //! 16-bit greyscale. Normally used for the alpha or dissolvemap.
        RedGreen,
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

    bool IsShuttingDown();
}