
#include <gtest/gtest.h>
#include "nexus/editor/FileDialog.h"

// To test the non-exported function ConstructFilterList, we include the implementation file.
// This is a common testing technique for private/static functions.
#if defined(NXS_PLATFORM_WIN64)
#include "engine/src/editor/FileDialog.Win64.cpp"
#endif

#include <string_view>
#include <cstring>

// Helper function to compare filter strings with embedded nulls
static bool CompareFilterStrings(const char* actual, const char* expected, size_t expectedSize)
{
    for (size_t i = 0; i < expectedSize; ++i)
    {
        if (actual[i] != expected[i])
            return false;
    }
    return true;
}

TEST(FileDialog, ConstructFilterList_Empty)
{
#if defined(NXS_PLATFORM_WIN64)
    std::vector<nxs::ExtFilter> filters = {};
    char filterStr[100] = {'x'}; // Initialize with non-zero to check null termination
    size_t count = ConstructFilterList(filters, filterStr, 100);

    EXPECT_EQ(count, 0);
    EXPECT_EQ(filterStr[0], '\0');
#else
    GTEST_SKIP() << "Test is for Windows only";
#endif
}

TEST(FileDialog, ConstructFilterList_SingleFilter)
{
#if defined(NXS_PLATFORM_WIN64)
    std::vector<nxs::ExtFilter> filters = {
        {"Image Files", "*.png;*.jpg"}
    };

    char filterStr[100];
    memset(filterStr, 0xFF, sizeof(filterStr)); // Fill with non-zero to check null termination

    size_t count = ConstructFilterList(filters, filterStr, 100);
    EXPECT_EQ(count, 1);

    const char* expected = "Image Files\0*.png;*.jpg\0";
    const auto expectedSize = sizeof("Image Files\0*.png;*.jpg\0");
    EXPECT_TRUE(CompareFilterStrings(filterStr, expected, expectedSize));

    // Check for double null termination
    EXPECT_EQ(filterStr[expectedSize - 1], '\0');
#else
    GTEST_SKIP() << "Test is for Windows only";
#endif
}

TEST(FileDialog, ConstructFilterList_MultipleFilters)
{
#if defined(NXS_PLATFORM_WIN64)
    std::vector<nxs::ExtFilter> filters = {
        {"Image Files", "*.png;*.jpg"},
        {"Text Files", "*.txt"}
    };

    char filterStr[100];
    memset(filterStr, 0xFF, sizeof(filterStr)); // Fill with non-zero to check null termination

    size_t count = ConstructFilterList(filters, filterStr, 100);
    EXPECT_EQ(count, 2);

    // The filter string has embedded null characters, so we use string_view for comparison.
    const char* expected = "Image Files\0*.png;*.jpg\0Text Files\0*.txt\0";
    const auto expectedSize = sizeof("Image Files\0*.png;*.jpg\0Text Files\0*.txt\0");
    EXPECT_TRUE(CompareFilterStrings(filterStr, expected, expectedSize));

    // Check for double null termination
    EXPECT_EQ(filterStr[expectedSize - 1], '\0');
#else
    GTEST_SKIP() << "Test is for Windows only";
#endif
}

// Test buffer size limit. The function must stop adding filters when the buffer is full.
TEST(FileDialog, ConstructFilterList_BufferTooSmall)
{
#if defined(NXS_PLATFORM_WIN64)
    std::vector<nxs::ExtFilter> filters = {
        {"Image Files", "*.png;*.jpg"},
        {"Text Files", "*.txt"}
    };

    char filterStr[30];
    memset(filterStr, 0xFF, sizeof(filterStr)); // Fill with non-zero to check null termination
    
    size_t count = ConstructFilterList(filters, filterStr, 30);
    EXPECT_EQ(count, 1);

    const char* expected = "Image Files\0*.png;*.jpg\0";
    const auto expectedSize = sizeof("Image Files\0*.png;*.jpg\0");
    EXPECT_TRUE(CompareFilterStrings(filterStr, expected, expectedSize));

    // Check for double null termination
    EXPECT_EQ(filterStr[expectedSize - 1], '\0');
#else
    GTEST_SKIP() << "Test is for Windows only";
#endif
}
