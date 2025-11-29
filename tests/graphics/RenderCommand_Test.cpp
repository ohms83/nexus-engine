#include "nexus/Nexus.h"
#include "gtest/gtest.h"

using namespace nxs;

TEST(RenderCommand, SortKey_DepthOrder)
{
    SortKey k1 = SortKey::From(false, 5, 1u);
    SortKey k2 = SortKey::From(false, 5, 2u);
    EXPECT_TRUE(k1 < k2);
}

TEST(RenderCommand, SortKey_Translucent)
{
    SortKey opaque = SortKey::From(false, 10, 0u);
    SortKey translucent = SortKey::From(true, 10, 0u);
    EXPECT_TRUE(opaque < translucent); // opaque should draw before translucent (lower key)
}
