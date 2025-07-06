set(ENGINE_PUBLIC_HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(ENGINE_INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)

set(ENGINE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(ENGINE_3RD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty)

set(ENGINE_SOURCES)
set(ENGINE_HEADERS)

add_subdirectory(3rdparty EXCLUDE_FROM_ALL)

include(src/CMakeLists.txt)
include(include/CMakeLists.txt)

add_library(nexus-engine
    ${ENGINE_SOURCES}
    ${ENGINE_HEADERS}
)

target_include_directories(nexus-engine
    PUBLIC ${ENGINE_INCLUDE_DIR}
    PRIVATE ${ENGINE_INCLUDE_DIR}/nexus

    PUBLIC ${ENGINE_3RD_PARTY_DIR}/SDL/include
)

target_link_libraries(nexus-engine PRIVATE SDL3::SDL3)
target_link_libraries(nexus-engine PRIVATE glad)

find_package(OpenGL REQUIRED)
# Link OpenGL library
target_link_libraries(nexus-engine PRIVATE
    OpenGL::GL
)

install(TARGETS nexus-engine
    LIBRARY DESTINATION lib)