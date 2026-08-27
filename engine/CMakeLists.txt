# Use cmake_path to get the absolute path
get_filename_component(PROJECT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)

set(ENGINE_PUBLIC_HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(ENGINE_MODULE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/modules)
set(ENGINE_INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)

set(ENGINE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(ENGINE_3RD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty)

set(TOOLS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tools)

add_subdirectory(3rdparty EXCLUDE_FROM_ALL)
add_subdirectory(modules)

# Third-party sources used by the engine modules.
set(IMGUI_SOURCE_DIR "${ENGINE_3RD_PARTY_DIR}/imgui" CACHE PATH "Path to Dear ImGui source root")
set(IMGUI_SOURCES
    "${IMGUI_SOURCE_DIR}/imgui.cpp"
    "${IMGUI_SOURCE_DIR}/imgui_draw.cpp"
    "${IMGUI_SOURCE_DIR}/imgui_widgets.cpp"
    "${IMGUI_SOURCE_DIR}/imgui_tables.cpp"
    "${IMGUI_SOURCE_DIR}/imgui_demo.cpp"
    "${IMGUI_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp"
    "${IMGUI_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp"
)
set(IMPLOT_SOURCE_DIR "${ENGINE_3RD_PARTY_DIR}/implot" CACHE PATH "Path to ImPlot source root")
set(IMPLOT_SOURCES
    "${IMPLOT_SOURCE_DIR}/implot.cpp"
    "${IMPLOT_SOURCE_DIR}/implot_demo.cpp"
    "${IMPLOT_SOURCE_DIR}/implot_items.cpp"
)
set(XXHASH_SOURCE_DIR "${ENGINE_3RD_PARTY_DIR}/xxHash" CACHE PATH "Path to xxHash source root")
set(STB_SOURCE_DIR "${ENGINE_3RD_PARTY_DIR}/stb" CACHE PATH "Path to stb_image source root")
set(REMOTERY_SOURCE_DIR ${ENGINE_3RD_PARTY_DIR}/Remotery/lib)
set(ENGINE_THIRD_PARTY_SOURCES
    ${IMGUI_SOURCES}
    ${IMPLOT_SOURCES}
    "${STB_SOURCE_DIR}/stb_image.cpp"
    "${REMOTERY_SOURCE_DIR}/Remotery.c"
)

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
    target_compile_definitions(${TARGET_NAME}
        PUBLIC NXS_PROJECT_PATH="${PROJECT_ROOT}"
        PUBLIC NXS_ASSETS_DIR="${PROJECT_ROOT}/assets"
        PUBLIC NXS_ENGINE_ASSETS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets"
        PUBLIC NXS_GENERATED_ASSETS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/gen"
        PUBLIC $<$<CONFIG:Debug>:NXS_BUILD_DEBUG>
        PUBLIC $<$<CONFIG:RelWithDebInfo>:NXS_BUILD_RELWITHDEBINFO>
        PUBLIC $<$<CONFIG:Release>:NXS_BUILD_RELEASE>
    )
    if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
        target_compile_definitions(${TARGET_NAME} PUBLIC NXS_PLATFORM_WIN64)
    endif()
endfunction()

file(GLOB_RECURSE ENGINE_FOUNDATION_SOURCES CONFIGURE_DEPENDS
    src/NxsDefine.cpp
    src/core/*.cpp
    src/ecs/*.cpp
    src/geom/*.cpp
    src/io/*.cpp
    src/memory/*.cpp
    src/time/*.cpp
)
add_library(nexus-core STATIC ${ENGINE_FOUNDATION_SOURCES})
nexus_target_defaults(nexus-core)
target_link_libraries(nexus-core
    PUBLIC EnTT
    PRIVATE SDL3::SDL3
    PRIVATE RapidJSON
    PRIVATE msgpack-cxx
)

file(GLOB_RECURSE ENGINE_GRAPHICS_SOURCES CONFIGURE_DEPENDS src/graphics/*.cpp)
add_library(nexus-graphics STATIC
    ${ENGINE_GRAPHICS_SOURCES}
    ${ENGINE_THIRD_PARTY_SOURCES}
)
nexus_target_defaults(nexus-graphics)
target_compile_definitions(nexus-graphics
    PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD
    PRIVATE RMT_USE_OPENGL=1
)
target_link_libraries(nexus-graphics
    PUBLIC nexus-core
    PUBLIC nexus-math
    PUBLIC SDL3::SDL3
    PUBLIC glad
    PUBLIC OpenGL::GL
    PUBLIC assimp::assimp
)

file(GLOB_RECURSE ENGINE_SCENE_SOURCES CONFIGURE_DEPENDS src/scene/*.cpp)
add_library(nexus-scene STATIC ${ENGINE_SCENE_SOURCES})
nexus_target_defaults(nexus-scene)
target_link_libraries(nexus-scene PUBLIC nexus-graphics)

file(GLOB_RECURSE ENGINE_EDITOR_MODULE_SOURCES CONFIGURE_DEPENDS src/editor/*.cpp)
add_library(nexus-editor-module STATIC ${ENGINE_EDITOR_MODULE_SOURCES})
nexus_target_defaults(nexus-editor-module)
target_link_libraries(nexus-editor-module PUBLIC nexus-scene)

add_library(nexus-engine STATIC
    src/Engine.cpp
    src/Application.cpp
)
nexus_target_defaults(nexus-engine)
target_link_libraries(nexus-engine
    PUBLIC nexus-editor-module
    PUBLIC nexus-math)

message("CMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}")

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_libraries(nexus-engine PRIVATE ws2_32)
    target_compile_definitions(nexus-engine PUBLIC NXS_PLATFORM_WIN64)
endif()

install(TARGETS nexus-engine
    LIBRARY DESTINATION lib)