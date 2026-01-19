file(GLOB_RECURSE TEST_SOURCES *.cpp)
file(GLOB_RECURSE TEST_HEADERS *.h)

# Google Test
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.17.0
)
# Make googletest's targets available (e.g., googletest::gtest_main)
FetchContent_MakeAvailable(googletest)

add_executable(nexus-test
    ${TEST_HEADERS}
    ${TEST_SOURCES}
)

target_include_directories(nexus-test
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# 2. Link to Google Test's main library
# googletest::gtest_main provides the main() function that runs all tests.
target_link_libraries(nexus-test PRIVATE
    nexus-engine
    GTest::gtest_main # Provides main() for Google Test
)

target_compile_definitions(nexus-test
    PRIVATE TEST_DATA_FOLDER="${CMAKE_CURRENT_SOURCE_DIR}/test_data"
)

# Runs the test suites with Google Test's --gtest_list_tests flag
# during CMake configure time. This creates individual CTest tests
# for each TEST() or TEST_F() defined in the source files.
include(GoogleTest)
gtest_discover_tests(nexus-test)