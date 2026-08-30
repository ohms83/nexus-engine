# A public include directory shared by every modules
set(NXS_PUBLIC_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(debug)
add_subdirectory(geom)
add_subdirectory(io)
add_subdirectory(math)
add_subdirectory(memory)
add_subdirectory(serialize)
add_subdirectory(reflection)
add_subdirectory(resource)
add_subdirectory(task)
add_subdirectory(time)