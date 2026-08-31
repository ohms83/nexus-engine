# Use cmake_path to get the absolute path
get_filename_component(PROJECT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)

set(ENGINE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(ENGINE_MODULE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/modules)
set(ENGINE_INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)

set(ENGINE_3RD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty)

set(TOOLS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tools)

# Third-party sources used by the engine modules.
set(IMGUI_SOURCE_DIR "${ENGINE_3RD_PARTY_DIR}/imgui" CACHE PATH "Path to Dear ImGui source root")
set(IMPLOT_SOURCE_DIR "${ENGINE_3RD_PARTY_DIR}/implot" CACHE PATH "Path to ImPlot source root")
set(XXHASH_SOURCE_DIR "${ENGINE_3RD_PARTY_DIR}/xxHash" CACHE PATH "Path to xxHash source root")
set(STB_SOURCE_DIR "${ENGINE_3RD_PARTY_DIR}/stb" CACHE PATH "Path to stb_image source root")
set(REMOTERY_SOURCE_DIR ${ENGINE_3RD_PARTY_DIR}/Remotery/lib)

add_subdirectory(3rdparty EXCLUDE_FROM_ALL)

find_package(Python3 COMPONENTS Interpreter REQUIRED)
find_package(OpenGL REQUIRED)

function(nexus_target_defaults TARGET_NAME)
    target_include_directories(${TARGET_NAME}
        PUBLIC ${ENGINE_INCLUDE_DIR}
        PRIVATE ${ENGINE_INCLUDE_DIR}/nexus
        PUBLIC ${ENGINE_3RD_PARTY_DIR}
        PUBLIC ${ENGINE_3RD_PARTY_DIR}/SDL/include
        PUBLIC ${ENGINE_3RD_PARTY_DIR}/glad/include
        PRIVATE ${ENGINE_3RD_PARTY_DIR}/rapidjson/include
        PUBLIC "${IMGUI_SOURCE_DIR}"
        PUBLIC "${IMGUI_SOURCE_DIR}/backends"
        PUBLIC "${IMPLOT_SOURCE_DIR}"
        PRIVATE "${XXHASH_SOURCE_DIR}"
        PRIVATE "${STB_SOURCE_DIR}"
        PRIVATE "${REMOTERY_SOURCE_DIR}"
    )
    # Link a single INTERFACE target that provides common compile definitions
    # and include directories to all engine targets.
    target_link_libraries(${TARGET_NAME} PUBLIC nexus-config)
    # Platform-specific defines are provided by the nexus-config INTERFACE target.
endfunction()

# Shared compile definitions / include dirs for all engine modules.
add_library(nexus-config INTERFACE)
target_compile_definitions(nexus-config INTERFACE
    NXS_PROJECT_PATH="${PROJECT_ROOT}"
    NXS_ASSETS_DIR="${PROJECT_ROOT}/assets"
    NXS_ENGINE_ASSETS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets"
    NXS_GENERATED_ASSETS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/gen"
    $<$<CONFIG:Debug>:NXS_BUILD_DEBUG>
    $<$<CONFIG:RelWithDebInfo>:NXS_BUILD_RELWITHDEBINFO>
    $<$<CONFIG:Release>:NXS_BUILD_RELEASE>
)
target_include_directories(nexus-config INTERFACE
    ${ENGINE_INCLUDE_DIR}
    ${ENGINE_3RD_PARTY_DIR}
)
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_compile_definitions(nexus-config INTERFACE NXS_PLATFORM_WIN64)
endif()

add_subdirectory(modules)

file(GLOB_RECURSE ENGINE_SCENE_SOURCES CONFIGURE_DEPENDS src/scene/*.cpp)
add_library(nexus-scene STATIC ${ENGINE_SCENE_SOURCES})
nexus_target_defaults(nexus-scene)
target_link_libraries(nexus-scene
    PUBLIC nexus-ecs
    PUBLIC nexus-graphics
    PUBLIC nexus-geom
)

file(GLOB_RECURSE ENGINE_EDITOR_MODULE_SOURCES CONFIGURE_DEPENDS src/editor/*.cpp)
add_library(nexus-editor-module STATIC ${ENGINE_EDITOR_MODULE_SOURCES})
nexus_target_defaults(nexus-editor-module)
target_link_libraries(nexus-editor-module
    PUBLIC nexus-scene
    PUBLIC nexus-3rdparty
    PRIVATE assimp::assimp
)

add_library(nexus-engine STATIC
    src/Engine.cpp
    src/Application.cpp
)
nexus_target_defaults(nexus-engine)
target_link_libraries(nexus-engine
    PUBLIC nexus-editor-module
    PUBLIC nexus-math
    PUBLIC nexus-core
    PUBLIC nexus-io
    PUBLIC nexus-geom
    PUBLIC nexus-graphics
    PUBLIC nexus-serialize
    PUBLIC nexus-scene
    PUBLIC nexus-3rdparty
)

message("CMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}")

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_libraries(nexus-engine PRIVATE ws2_32)
    target_compile_definitions(nexus-engine PUBLIC NXS_PLATFORM_WIN64)
endif()

install(TARGETS nexus-engine
    LIBRARY DESTINATION lib)