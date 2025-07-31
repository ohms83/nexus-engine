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