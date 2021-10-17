cmake_minimum_required(VERSION 3.10)

# TODO: Find a better name
project(01-HelloWorld)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)

add_executable(${PROJECT_NAME} main.cpp)

# GLM
target_include_directories(${PROJECT_NAME}
    PUBLIC "${CMAKE_SOURCE_DIR}/nexus/thirdparty"
)

# OpenGL
set(OpenGL_GL_PREFERENCE GLVND) 
find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} GL)

# SDL2
find_package(SDL2 REQUIRED)
target_link_libraries(${PROJECT_NAME} SDL2)