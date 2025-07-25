set(SDL_SHARED OFF)
set(SDL_STATIC ON)
add_subdirectory(SDL)

add_subdirectory(glad)
add_subdirectory(entt EXCLUDE_FROM_ALL)