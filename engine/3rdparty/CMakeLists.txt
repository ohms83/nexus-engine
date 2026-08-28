set(SDL_SHARED OFF)
set(SDL_STATIC ON)
add_subdirectory(SDL)

add_subdirectory(glad EXCLUDE_FROM_ALL)
add_subdirectory(entt EXCLUDE_FROM_ALL)

set(RAPIDJSON_BUILD_EXAMPLES OFF CACHE BOOL "Do not build rapidjson examples" FORCE)
set(RAPIDJSON_BUILD_CXX20 ON CACHE BOOL "Build rapidjson with C++20" FORCE)
set(RAPIDJSON_BUILD_TESTS OFF CACHE BOOL "Do not build rapidjson perftests and unittests" FORCE)
set(RAPIDJSON_BUILD_DOC OFF CACHE BOOL "Do not build rapidjson documentation." FORCE)
add_subdirectory(rapidjson)

# MessagePack
FetchContent_Declare(
  msgpackcxx
  GIT_REPOSITORY https://github.com/ohms83/msgpack-c.git
  GIT_TAG        cpp-7.0.0 # Or the latest stable release tag you want
)

# Fetch the content (downloads if not already downloaded)
set(MSGPACK_USE_BOOST OFF CACHE BOOL "Turn off boost dependency. Note: the test projects won't be able to compile" FORCE)
FetchContent_MakeAvailable(msgpackcxx)

# Assimp
FetchContent_Declare(
  assimp_proj
  GIT_REPOSITORY https://github.com/assimp/assimp.git
  GIT_TAG        master
  GIT_SHALLOW    TRUE   # For faster cloning, requires Git 2.15+
)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static library" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "Disable building tests" FORCE)
set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "Do not treat Assimp warnings as hard errors" FORCE)
FetchContent_MakeAvailable(assimp_proj)