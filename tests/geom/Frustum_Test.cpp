#include "gtest/gtest.h"
#include "nexus/Nexus.h"

USING_NAMESPACE_NXS;

static Frustum MakeUnitCubeFrustum()
{
    Frustum f;
    // Left, Right, Bottom, Top, Near, Far (axis-aligned cube [-1,1]^3)
    f.planes[0] = Plane(glm::vec3(1,0,0), glm::vec3(-1,0,0));  // x >= -1
    f.planes[1] = Plane(glm::vec3(-1,0,0), glm::vec3(1,0,0));  // x <=  1
    f.planes[2] = Plane(glm::vec3(0,1,0), glm::vec3(0,-1,0));  // y >= -1
    f.planes[3] = Plane(glm::vec3(0,-1,0), glm::vec3(0,1,0));  // y <=  1
    f.planes[4] = Plane(glm::vec3(0,0,1), glm::vec3(0,0,-1));  // z >= -1
    f.planes[5] = Plane(glm::vec3(0,0,-1), glm::vec3(0,0,1));  // z <=  1
    return f;
}

TEST(Frustum_PointInside, CenterAndBoundary) {
    Frustum f = MakeUnitCubeFrustum();
    EXPECT_TRUE(f.IsPointInside(glm::vec3(0.0f, 0.0f, 0.0f))); // center
    EXPECT_TRUE(f.IsPointInside(glm::vec3(1.0f, 0.0f, 0.0f))); // on boundary -> inside
    EXPECT_FALSE(f.IsPointInside(glm::vec3(2.0f, 0.0f, 0.0f))); // outside
}

TEST(Frustum_SphereInside, FullyInside) {
    Frustum f = MakeUnitCubeFrustum();
    EXPECT_TRUE(f.IsSphereInside(glm::vec3(0.0f), 0.5f));
}

TEST(Frustum_SphereInside, FullyOutside) {
    Frustum f = MakeUnitCubeFrustum();
    EXPECT_FALSE(f.IsSphereInside(glm::vec3(2.0f, 0.0f, 0.0f), 0.5f));
}

TEST(Frustum_SphereInside, PartialIntersection) {
    Frustum f = MakeUnitCubeFrustum();
    // Center at 1.4 on x with radius 0.5 -> partially intersects right plane
    EXPECT_TRUE(f.IsSphereInside(glm::vec3(1.4f, 0.0f, 0.0f), 0.5f));
}

TEST(Frustum_SphereInside, TangentReturnsTrue) {
    Frustum f = MakeUnitCubeFrustum();
    // Center at 1.5 with radius 0.5 -> tangent at x=1, should be considered inside
    EXPECT_TRUE(f.IsSphereInside(glm::vec3(1.5f, 0.0f, 0.0f), 0.5f));
}

TEST(Frustum_SphereInside, ZeroRadiusReturnsFalse) {
    Frustum f = MakeUnitCubeFrustum();
    EXPECT_FALSE(f.IsSphereInside(glm::vec3(0.0f), 0.0f));
}