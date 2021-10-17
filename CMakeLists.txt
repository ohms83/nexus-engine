cmake_minimum_required(VERSION 3.10)

project(Nexus-Engine VERSION 0.1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)

include(CTest)
enable_testing()

if(UNIX AND NOT APPLE)
    # Use a combination of UNIX and NOT APPLE to specify Linux since there's
    # no specific variable for Linux
    set(LINUX true)
endif()

add_subdirectory(nexus)
add_subdirectory(examples/01-HelloWorld)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
