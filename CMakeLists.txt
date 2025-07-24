cmake_minimum_required(VERSION 3.31)
project(Nexus)

# Enable CTest
enable_testing()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)

message("CMAKE_BUILD_TYPE  ${CMAKE_BUILD_TYPE}")

set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR})

# --- Define resource directory ---
set(ASSETS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets")

set(CONFIG_SUBDIR "$<IF:$<CONFIG:Debug>,Debug,$<IF:$<CONFIG:Release>,Release,>>")
# Copy the resources next to the exe for all configs:
set(ASSETS_DEST_DIR "${CMAKE_CURRENT_BINARY_DIR}/${CONFIG_SUBDIR}")

# Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  # Use a specific release tag
  URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
)
# Make googletest's targets available (e.g., googletest::gtest_main)
FetchContent_MakeAvailable(googletest)

# Flecs
FetchContent_Declare(
  flecs
  GIT_REPOSITORY https://github.com/SanderMertens/flecs.git
  GIT_TAG        release-3.2.0 # Or any other specific release tag, e.g., "master" for latest
)
FetchContent_MakeAvailable(flecs)

add_subdirectory(engine)
add_subdirectory(examples)
add_subdirectory(editor)
add_subdirectory(tests)
