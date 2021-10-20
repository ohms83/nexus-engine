cmake_minimum_required(VERSION 3.10)

project(02-FirstPolygon)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)

add_executable(${PROJECT_NAME} main.cpp)

find_library(engine ${CMAKE_SOURCE_DIR}/nexus)
target_link_libraries(${PROJECT_NAME} Nexus)