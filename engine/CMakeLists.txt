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
    PRIVATE ${ENGINE_3RD_PARTY_DIR}/SDL/include
    PRIVATE ${ENGINE_3RD_PARTY_DIR}/bgfx.cmake/bgfx/include
    PRIVATE ${ENGINE_3RD_PARTY_DIR}/bgfx.cmake/bx/include
    PRIVATE ${ENGINE_3RD_PARTY_DIR}/bgfx.cmake/bimg/include
)

target_link_libraries(nexus-engine SDL3::SDL3)
target_link_libraries(nexus-engine bgfx)
target_link_libraries(nexus-engine bx)
target_link_libraries(nexus-engine bimg)

install(TARGETS nexus-engine
    LIBRARY DESTINATION lib)