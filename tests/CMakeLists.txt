add_executable(nexus-test
    scene/Transform_Test.cpp
)

# 2. Link to Google Test's main library
# googletest::gtest_main provides the main() function that runs all tests.
target_link_libraries(nexus-test PRIVATE
    nexus-engine
    GTest::gtest_main # Provides main() for Google Test
)

# Runs the test suites with Google Test's --gtest_list_tests flag
# during CMake configure time. This creates individual CTest tests
# for each TEST() or TEST_F() defined in the source files.
include(GoogleTest)
gtest_discover_tests(nexus-test)