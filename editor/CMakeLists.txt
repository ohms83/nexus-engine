set(EDITOR_SOURCES
    NexusEditor.cpp
    NexusEditor.h
)

add_executable(nexus-editor ${EDITOR_SOURCES})

target_compile_definitions(nexus-editor
    PRIVATE ASSETS_DIR="${ASSETS_DIR}")
message("ASSETS_DIR=${ASSETS_DIR}")
target_link_libraries(nexus-editor nexus-engine)