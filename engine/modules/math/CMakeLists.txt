set(TARGET_NAME nexus-math)
set(NXS_MATH_PUBLIC_HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB_RECURSE NXS_MATH_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_library(${TARGET_NAME} STATIC ${NXS_MATH_SOURCES})
target_include_directories(${TARGET_NAME}
    PUBLIC ${NXS_MATH_PUBLIC_HEADER_DIR}
)

target_link_libraries(${TARGET_NAME}
    PUBLIC nexus-config
)
