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

# SDL
target_link_libraries(${PROJECT_NAME}
    SDL2main
    SDL2_ttf
    SDL2_image
)

# SDL2
# find_package(SDL2 REQUIRED)
# target_link_libraries(${PROJECT_NAME} SDL2)

# Platform specific macros and libraries
if(WINDOWS)
    ADD_DEFINITIONS (-DTARGET_WINDOWS)
elseif(APPLE)
    target_link_directories(${PROJECT_NAME}
        PUBLIC "${ENGINE_ROOT}/thirdparty/lib/macos"
    )

    target_link_libraries(${PROJECT_NAME}
        libSDL2-2.0.0.dylib
    )

    if(IOS)
        ADD_DEFINITIONS (-DTARGET_IOS)
    else()
        ADD_DEFINITIONS (-DTARGET_MACOS)
    endif()
elseif(LINUX)
    ADD_DEFINITIONS (-DTARGET_LINUX)
elseif(ANDROID)
    ADD_DEFINITIONS (-DTARGET_ANDROID)
else()
    message( FATAL_ERROR "Unsupported platform, CMake will exit" )
endif()