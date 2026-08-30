set(TARGET_NAME nexus-geom)
set(NXS_GEOM_PUBLIC_HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB_RECURSE NXS_GEOM_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_library(${TARGET_NAME} STATIC ${NXS_GEOM_SOURCES})

target_include_directories(${TARGET_NAME}
    PUBLIC ${NXS_GEOM_PUBLIC_HEADER_DIR}
)

target_link_libraries(${TARGET_NAME}
    PUBLIC nexus-config
    PRIVATE nexus-debug
)
