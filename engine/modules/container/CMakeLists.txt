set(TARGET_NAME nexus-container)
set(NXS_CONTAINER_PUBLIC_HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(${TARGET_NAME} INTERFACE)

target_include_directories(${TARGET_NAME}
    INTERFACE ${NXS_CONTAINER_PUBLIC_HEADER_DIR}
)

target_link_libraries(${TARGET_NAME}
    INTERFACE nexus-config
    INTERFACE mimalloc-static
)
