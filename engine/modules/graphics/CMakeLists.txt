set(TARGET_NAME nexus-graphics)
set(NXS_GRAPHICS_PUBLIC_HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB_RECURSE NXS_GRAPHICS_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_library(${TARGET_NAME} STATIC ${NXS_GRAPHICS_SOURCES})

target_include_directories(${TARGET_NAME}
    PUBLIC ${NXS_GRAPHICS_PUBLIC_HEADER_DIR}

    PUBLIC ${ENGINE_3RD_PARTY_DIR}/SDL/include
    PUBLIC ${ENGINE_3RD_PARTY_DIR}/glad/include

    PRIVATE "${STB_SOURCE_DIR}"
    PRIVATE "${REMOTERY_SOURCE_DIR}"
)

target_link_libraries(${TARGET_NAME}
    # Public libraries that other modules can use
    PUBLIC nexus-config
    PUBLIC nexus-core
    PUBLIC nexus-math
    PUBLIC nexus-memory
    PUBLIC nexus-geom
    PUBLIC nexus-resource

    # Private libraries that are only used internally
    PRIVATE nexus-3rdparty
    PRIVATE nexus-time
    PRIVATE nexus-serialize

    PRIVATE SDL3::SDL3
    PRIVATE glad
    PRIVATE OpenGL::GL
    PRIVATE assimp::assimp
)
