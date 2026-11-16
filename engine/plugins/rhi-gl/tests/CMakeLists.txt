add_executable(nexus-rhi-opengl-test
    GLCall_Test.cpp
)

target_include_directories(nexus-rhi-opengl-test PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../include"
)

target_link_libraries(nexus-rhi-opengl-test PRIVATE
    nexus-graphics
    nexus-debug
    glad
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(nexus-rhi-opengl-test)
