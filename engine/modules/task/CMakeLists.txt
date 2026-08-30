set(TARGET_NAME nexus-task)
set(NXS_TASK_PUBLIC_HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB_RECURSE NXS_TASK_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_library(${TARGET_NAME} STATIC ${NXS_TASK_SOURCES})
target_include_directories(${TARGET_NAME}
    PUBLIC ${NXS_TASK_PUBLIC_HEADER_DIR}

    PRIVATE "${REMOTERY_SOURCE_DIR}"
)

target_link_libraries(${TARGET_NAME}
    PUBLIC nexus-config
    PUBLIC nexus-time
)
