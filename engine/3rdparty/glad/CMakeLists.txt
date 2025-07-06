cmake_minimum_required(VERSION 3.10)

project(glad)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)

add_library(${PROJECT_NAME} src/glad.c)

target_include_directories(${PROJECT_NAME} PUBLIC include)