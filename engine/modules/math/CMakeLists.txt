file(GLOB_RECURSE MATH_SOURCES *.cpp)
file(GLOB_RECURSE MATH_HEADERS *.h)

add_library(nexus-math SHARED
    ${MATH_SOURCES}
    ${MATH_HEADERS}
)

target_link_libraries(nexus-math PUBLIC SDL3::SDL3)

target_include_directories(nexus-math
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    PUBLIC ${ENGINE_3RD_PARTY_DIR}/SDL/include
)

target_link_libraries(nexus-math
    PRIVATE SDL3::SDL3
    PUBLIC  EnTT
)
set_target_properties(nexus-math PROPERTIES
    OUTPUT_NAME "nexus-math"
)