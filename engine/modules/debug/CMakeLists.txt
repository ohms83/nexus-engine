set(TARGET_NAME nexus-debug)
set(NXS_DEBUG_PUBLIC_HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB_RECURSE NXS_DEBUG_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_library(${TARGET_NAME} STATIC ${NXS_DEBUG_SOURCES})

target_include_directories(${TARGET_NAME}
    PUBLIC ${NXS_DEBUG_PUBLIC_HEADER_DIR}
)

target_link_libraries(${TARGET_NAME}
    PUBLIC nexus-config
)

