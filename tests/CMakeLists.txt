file(GLOB_RECURSE TEST_SOURCES *.cpp)
file(GLOB_RECURSE TEST_HEADERS *.h)

add_executable(nexus-test
    ${TEST_HEADERS}
    ${TEST_SOURCES}
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