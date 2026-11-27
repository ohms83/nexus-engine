#include "gtest/gtest.h"
#include "nexus/Nexus.h"

USING_NAMESPACE_NXS;

TEST(MathTest, ComputeMaxMipLevel)
{
    EXPECT_EQ(Math::ComputeMaxMipLevel(1), 1);
    EXPECT_EQ(Math::ComputeMaxMipLevel(128), 8);
    EXPECT_EQ(Math::ComputeMaxMipLevel(1920), 11);
    EXPECT_EQ(Math::ComputeMaxMipLevel(1920, 1080), 11);
    EXPECT_EQ(Math::ComputeMaxMipLevel(1920, 1921), 11);
}
