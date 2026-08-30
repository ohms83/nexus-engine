set(TARGET_NAME nexus-core)
set(NXS_CORE_PUBLIC_HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB_RECURSE NXS_CORE_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_library(${TARGET_NAME} STATIC ${NXS_CORE_SOURCES})
target_include_directories(${TARGET_NAME}
    PUBLIC ${NXS_CORE_PUBLIC_HEADER_DIR}
    PRIVATE "${XXHASH_SOURCE_DIR}"
)

target_link_libraries(${TARGET_NAME}
    PUBLIC nexus-config
    PUBLIC nexus-debug
)
