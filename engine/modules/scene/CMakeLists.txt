set(TARGET_NAME nexus-scene)
set(NXS_SCENE_PUBLIC_HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB_RECURSE NXS_SCENE_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_library(${TARGET_NAME} STATIC ${NXS_SCENE_SOURCES})
target_include_directories(${TARGET_NAME}
    PUBLIC ${NXS_SCENE_PUBLIC_HEADER_DIR}
    PRIVATE "${REMOTERY_SOURCE_DIR}"
)

target_link_libraries(${TARGET_NAME}
    PRIVATE nexus-config
    PRIVATE nexus-core
    PRIVATE nexus-debug
    PRIVATE nexus-3rdparty

    PUBLIC nexus-ecs
    PUBLIC nexus-graphics
    PUBLIC nexus-geom
)
