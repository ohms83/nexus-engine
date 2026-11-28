include(script/CMakeLists.txt)
include(utils/CMakeLists.txt)

set(EDITOR_SOURCES
    NexusEditor.cpp
    NexusEditor.h
)

add_executable(nexus-editor
    ${EDITOR_SOURCES}
    ${EDITOR_SCRIPT_SOURCES}
    ${EDITOR_UTILS_SOURCES})

target_compile_definitions(nexus-editor PRIVATE
    NXS_TOOLS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/tools"
)

target_link_libraries(nexus-editor nexus-engine)