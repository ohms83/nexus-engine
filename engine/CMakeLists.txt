set(ENGINE_PUBLIC_HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(ENGINE_INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)

set(ENGINE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(ENGINE_3RD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty)

set(ENGINE_SOURCES)
set(ENGINE_HEADERS)

add_subdirectory(3rdparty EXCLUDE_FROM_ALL)

# Dear-Imgui
set(IMGUI_SOURCE_DIR "${ENGINE_3RD_PARTY_DIR}/imgui" CACHE PATH "Path to Dear ImGui source root")

# Core ImGui sources
list(APPEND ENGINE_SOURCES
    "${IMGUI_SOURCE_DIR}/imgui.cpp"
    "${IMGUI_SOURCE_DIR}/imgui_draw.cpp"
    "${IMGUI_SOURCE_DIR}/imgui_widgets.cpp"
    "${IMGUI_SOURCE_DIR}/imgui_tables.cpp"
    "${IMGUI_SOURCE_DIR}/imgui_demo.cpp" # Include this for demo window, useful for testing
)
# ImGui backends for SDL3 and OpenGL3
list(APPEND ENGINE_SOURCES
    "${IMGUI_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp"
    "${IMGUI_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp"
)

# ImPlot
set(IMPLOT_SOURCE_DIR "${ENGINE_3RD_PARTY_DIR}/implot" CACHE PATH "Path to ImPlot source root")
list(APPEND ENGINE_SOURCES
    "${IMPLOT_SOURCE_DIR}/implot.cpp"
    "${IMPLOT_SOURCE_DIR}/implot_demo.cpp"
    "${IMPLOT_SOURCE_DIR}/implot_items.cpp"
)
list(APPEND ENGINE_HEADERS
    "${IMPLOT_SOURCE_DIR}/implot.h"
    "${IMPLOT_SOURCE_DIR}/implot_internal.h"
)

# xxHash
set(XXHASH_SOURCE_DIR "${ENGINE_3RD_PARTY_DIR}/xxHash" CACHE PATH "Path to xxHash source root")
list(APPEND ENGINE_HEADERS
    "${XXHASH_SOURCE_DIR}/xxhash.h"
)

# stb_image
set(STB_SOURCE_DIR "${ENGINE_3RD_PARTY_DIR}/stb" CACHE PATH "Path to stb_image source root")
list(APPEND ENGINE_HEADERS
    "${STB_SOURCE_DIR}/stb_image.h"
)
list(APPEND ENGINE_SOURCES
    "${STB_SOURCE_DIR}/stb_image.cpp"
)

include(src/CMakeLists.txt)
include(include/CMakeLists.txt)

add_library(nexus-engine
    ${ENGINE_SOURCES}
    ${ENGINE_HEADERS}
)

target_include_directories(nexus-engine
    PUBLIC ${ENGINE_INCLUDE_DIR}
    PRIVATE ${ENGINE_INCLUDE_DIR}/nexus

    # 3rd parties
    PUBLIC ${ENGINE_3RD_PARTY_DIR}/SDL/include
    PUBLIC ${ENGINE_3RD_PARTY_DIR}/glad/include
    # Dear-Imgui
    PUBLIC "${IMGUI_SOURCE_DIR}"
    PUBLIC "${IMGUI_SOURCE_DIR}/backends"
    # ImPlot
    PUBLIC "${IMPLOT_SOURCE_DIR}"
    # xxHash
    PUBLIC "${XXHASH_SOURCE_DIR}"
    # stb_image
    PUBLIC "${STB_SOURCE_DIR}"
)

target_link_libraries(nexus-engine PRIVATE SDL3::SDL3)
target_link_libraries(nexus-engine PRIVATE glad)

find_package(OpenGL REQUIRED)
# Link OpenGL library
target_link_libraries(nexus-engine PRIVATE
    OpenGL::GL
)

# Define IMGUI_IMPL_OPENGL_LOADER_GLAD for OpenGL3 backend to use glad
target_compile_definitions(nexus-engine
    PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD
)

install(TARGETS nexus-engine
    LIBRARY DESTINATION lib)