set(SDL_SHARED OFF)
set(SDL_STATIC ON)
add_subdirectory(SDL)

add_subdirectory(glad EXCLUDE_FROM_ALL)
add_subdirectory(entt EXCLUDE_FROM_ALL)

set(RAPIDJSON_BUILD_EXAMPLES OFF CACHE BOOL "Do not build rapidjson examples" FORCE)
set(RAPIDJSON_BUILD_CXX20 ON CACHE BOOL "Build rapidjson with C++20" FORCE)
set(RAPIDJSON_BUILD_TESTS OFF CACHE BOOL "Do not build rapidjson perftests and unittests" FORCE)
set(RAPIDJSON_BUILD_DOC OFF CACHE BOOL "Do not build rapidjson documentation." FORCE)
add_subdirectory(rapidjson)

# MessagePack
FetchContent_Declare(
  msgpackcxx
  GIT_REPOSITORY https://github.com/ohms83/msgpack-c.git
  GIT_TAG        cpp-7.0.0 # Or the latest stable release tag you want
)

# Fetch the content (downloads if not already downloaded)
set(MSGPACK_USE_BOOST OFF CACHE BOOL "Turn off boost dependency. Note: the test projects won't be able to compile" FORCE)
FetchContent_MakeAvailable(msgpackcxx)

# Assimp
FetchContent_Declare(
  assimp_proj
  GIT_REPOSITORY https://github.com/assimp/assimp.git
  GIT_TAG        master
  GIT_SHALLOW    TRUE   # For faster cloning, requires Git 2.15+
)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static library" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "Disable building tests" FORCE)
set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "Do not treat Assimp warnings as hard errors" FORCE)
FetchContent_MakeAvailable(assimp_proj)

set(IMGUI_SOURCES
    "${IMGUI_SOURCE_DIR}/imgui.cpp"
    "${IMGUI_SOURCE_DIR}/imgui_draw.cpp"
    "${IMGUI_SOURCE_DIR}/imgui_widgets.cpp"
    "${IMGUI_SOURCE_DIR}/imgui_tables.cpp"
    "${IMGUI_SOURCE_DIR}/imgui_demo.cpp"
    "${IMGUI_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp"
    "${IMGUI_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp"
)
set(IMPLOT_SOURCES
    "${IMPLOT_SOURCE_DIR}/implot.cpp"
    "${IMPLOT_SOURCE_DIR}/implot_demo.cpp"
    "${IMPLOT_SOURCE_DIR}/implot_items.cpp"
)
set(ENGINE_THIRD_PARTY_SOURCES
    ${IMGUI_SOURCES}
    ${IMPLOT_SOURCES}
    "${STB_SOURCE_DIR}/stb_image.cpp"
    "${REMOTERY_SOURCE_DIR}/Remotery.c"
)

# A static library containing sources from third-party libraries that don't have
# their own CMake build system. This library is linked to engine modules that need it.
add_library(nexus-3rdparty STATIC ${ENGINE_THIRD_PARTY_SOURCES})

# Each module that links to the nexus-3rdparty needs to manually include its required header by itself
# This is to avoid unnecessary includes for modules that don't need them,
# and to avoid polluting the global namespace with third-party headers.
target_include_directories(nexus-3rdparty
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/SDL/include"
    PRIVATE "${IMGUI_SOURCE_DIR}"
    PRIVATE "${IMGUI_SOURCE_DIR}/backends"
    PRIVATE "${IMPLOT_SOURCE_DIR}"
    PRIVATE "${XXHASH_SOURCE_DIR}"
    PRIVATE "${STB_SOURCE_DIR}"
    PRIVATE "${REMOTERY_SOURCE_DIR}"
)

target_compile_definitions(nexus-3rdparty
    PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD
    PRIVATE RMT_USE_OPENGL=1
)
