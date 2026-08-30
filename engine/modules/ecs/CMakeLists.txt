set(TARGET_NAME nexus-ecs)
set(NXS_ECS_PUBLIC_HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB_RECURSE NXS_ECS_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_library(${TARGET_NAME} STATIC ${NXS_ECS_SOURCES})

target_include_directories(${TARGET_NAME}
    PUBLIC ${NXS_ECS_PUBLIC_HEADER_DIR}
)

target_link_libraries(${TARGET_NAME}
    PRIVATE nexus-debug

    PUBLIC nexus-config
    PUBLIC nexus-reflection
    PUBLIC EnTT
)
