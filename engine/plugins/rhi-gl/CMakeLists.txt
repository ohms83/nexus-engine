set(TARGET_NAME nexus-rhi-opengl)

file(GLOB_RECURSE NXS_RHI_OPENGL_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_library(${TARGET_NAME} SHARED ${NXS_RHI_OPENGL_SOURCES})

target_include_directories(${TARGET_NAME}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${ENGINE_3RD_PARTY_DIR}/glad/include
    PRIVATE "${REMOTERY_SOURCE_DIR}"
)

target_link_libraries(${TARGET_NAME}
    # Links against the core RHI interface (brings in SDL3, nexus-core, etc. transitively)
    PRIVATE nexus-graphics
    PRIVATE SDL3::SDL3
    PRIVATE glad
    PRIVATE OpenGL::GL
)

# Output the plugin binary into <build_dir>/bin/plugins/ (or alongside your main executable)
set_target_properties(${TARGET_NAME} PROPERTIES
    PREFIX "" # Prevents "lib" prefix on Linux/macOS (outputs nexus-rhi-opengl.so / .dylib)
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins"
)