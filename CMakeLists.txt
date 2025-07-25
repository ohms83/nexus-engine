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

include(FetchContent)

# Google Test
FetchContent_Declare(
  googletest
  SOURCE_DIR "${CMAKE_SOURCE_DIR}/_deps/googletest-src"
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.17.0
)
# Make googletest's targets available (e.g., googletest::gtest_main)
FetchContent_MakeAvailable(googletest)

# Flecs
FetchContent_Declare(
  flecs
  SOURCE_DIR "${CMAKE_SOURCE_DIR}/_deps/flecs-src"
  GIT_REPOSITORY https://github.com/SanderMertens/flecs.git
  GIT_TAG        master
  # Pass CMake options to the fetched Flecs project.
  # These are crucial for configuring how Flecs itself is built.
  CMAKE_ARGS
    -DFLECS_SHARED=OFF    # Build Flecs as a static library (recommended for easier integration)
    -DFLECS_CPP=ON        # Enable the C++ API (essential for C++ projects)
    -DFLECS_PIPELINE=ON   # Include the built-in pipeline module
    -DFLECS_REST=OFF       # Include the REST API module (useful for web-based tooling)
    -DFLECS_SYSTEM=ON     # Include the system module
    -DFLECS_MODULE=ON     # Include module support
    # -DFLECS_LUA=ON      # Lua scripting support
)
FetchContent_MakeAvailable(flecs)

add_subdirectory(engine)
add_subdirectory(examples)
add_subdirectory(editor)
add_subdirectory(tests)
