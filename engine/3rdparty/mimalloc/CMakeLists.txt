# Build mimalloc as a static allocator for engine consumers.
FetchContent_Declare(
  mimalloc
  GIT_REPOSITORY https://github.com/microsoft/mimalloc.git
  GIT_TAG        v3.5.1
  GIT_SHALLOW    TRUE
)
set(MI_BUILD_SHARED OFF CACHE BOOL "Build mimalloc as a shared library" FORCE)
set(MI_BUILD_STATIC ON CACHE BOOL "Build mimalloc as a static library" FORCE)
set(MI_BUILD_OBJECT OFF CACHE BOOL "Build mimalloc as an object library" FORCE)
set(MI_BUILD_TESTS OFF CACHE BOOL "Build mimalloc tests" FORCE)
set(MI_OVERRIDE ON CACHE BOOL "Override the standard C malloc interface" FORCE)
FetchContent_MakeAvailable(mimalloc)