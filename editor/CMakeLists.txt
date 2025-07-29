set(EDITOR_SOURCES
    NexusEditor.cpp
    NexusEditor.h
)

add_executable(nexus-editor ${EDITOR_SOURCES})

target_link_libraries(nexus-editor nexus-engine)