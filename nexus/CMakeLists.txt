cmake_minimum_required(VERSION 3.10)

# TODO: Find a better name
project(Nexus)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)

set(ENGINE_SOURCE)
set(ENGINE_HEADER)

set(ENGINE_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

include(base/CMakeLists.txt)
include(graphics/CMakeLists.txt)
include(io/CMakeLists.txt)
include(os/CMakeLists.txt)
include(pattern/CMakeLists.txt)
include(platform/CMakeLists.txt)

add_library(${PROJECT_NAME}
    ${ENGINE_SOURCE}
    ${ENGINE_HEADER}
)

target_include_directories(
    ${PROJECT_NAME} PUBLIC "${ENGINE_ROOT}"
    ${PROJECT_NAME} PUBLIC "${ENGINE_ROOT}/thirdparty/include"
)

# OpenGL
set(OpenGL_GL_PREFERENCE GLVND) 
find_package(OpenGL REQUIRED)
message(STATUS "OPENGL_INCLUDE_DIR: ${OPENGL_INCLUDE_DIR}")
include_directories(${OPENGL_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME}
    GL
)

# SDL
find_package(SDL2 REQUIRED)
message(STATUS "SDL2_INCLUDE_DIRS: ${SDL2_INCLUDE_DIRS}")
include_directories(${SDL2_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME}
    SDL2
    SDL2main
    SDL2_ttf
    SDL2_image
)

set(TOOLS_DIR "${CMAKE_SOURCE_DIR}/tools")

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    
    COMMAND python3 ${TOOLS_DIR}/generate-header-list.py
)