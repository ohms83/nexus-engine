#include "gtest/gtest.h"
#include "nexus/geom/Collider.h"

USING_NAMESPACE_NXS;

static Frustum MakeColliderTestFrustum()
{
    Frustum frustum;
    frustum.planes[0] = Plane(glm::vec3(1, 0, 0), glm::vec3(-1, 0, 0));
    frustum.planes[1] = Plane(glm::vec3(-1, 0, 0), glm::vec3(1, 0, 0));
    frustum.planes[2] = Plane(glm::vec3(0, 1, 0), glm::vec3(0, -1, 0));
    frustum.planes[3] = Plane(glm::vec3(0, -1, 0), glm::vec3(0, 1, 0));
    frustum.planes[4] = Plane(glm::vec3(0, 0, 1), glm::vec3(0, 0, -1));
    frustum.planes[5] = Plane(glm::vec3(0, 0, -1), glm::vec3(0, 0, 1));
    return frustum;
}

TEST(ColliderTest, PointInsideBoxIncludesBoundary)
{
    const Box box(glm::vec3(0.0f), glm::vec3(1.0f));

    EXPECT_TRUE(Collider::IsInside(glm::vec3(0.0f), box));
    EXPECT_TRUE(Collider::IsInside(glm::vec3(1.0f, -1.0f, 0.5f), box));
    EXPECT_FALSE(Collider::IsInside(glm::vec3(1.01f, 0.0f, 0.0f), box));
}

TEST(ColliderTest, PointInsideSphereIncludesBoundary)
{
    const Sphere sphere(glm::vec3(0.0f), 2.0f);

    EXPECT_TRUE(Collider::IsInside(glm::vec3(0.0f), sphere));
    EXPECT_TRUE(Collider::IsInside(glm::vec3(2.0f, 0.0f, 0.0f), sphere));
    EXPECT_FALSE(Collider::IsInside(glm::vec3(2.01f, 0.0f, 0.0f), sphere));
    EXPECT_FALSE(Collider::IsInside(glm::vec3(0.0f), Sphere(glm::vec3(0.0f), -1.0f)));
}

TEST(ColliderTest, PointInsideFrustumIncludesBoundary)
{
    const auto frustum = MakeColliderTestFrustum();

    EXPECT_TRUE(Collider::IsInside(glm::vec3(0.0f), frustum));
    EXPECT_TRUE(Collider::IsInside(glm::vec3(1.0f, 0.0f, 0.0f), frustum));
    EXPECT_FALSE(Collider::IsInside(glm::vec3(1.01f, 0.0f, 0.0f), frustum));
}

TEST(ColliderTest, BoxBoxIntersectionIncludesTouchingFaces)
{
    const Box boxA(glm::vec3(0.0f), glm::vec3(1.0f));

    EXPECT_TRUE(Collider::IsIntersect(boxA, Box(glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(1.0f))));
    EXPECT_TRUE(Collider::IsIntersect(boxA, Box(glm::vec3(1.5f, 0.0f, 0.0f), glm::vec3(1.0f))));
    EXPECT_FALSE(Collider::IsIntersect(boxA, Box(glm::vec3(2.01f, 0.0f, 0.0f), glm::vec3(1.0f))));
}

TEST(ColliderTest, BoxSphereIntersectionUsesClosestPoint)
{
    const Box box(glm::vec3(0.0f), glm::vec3(1.0f));

    EXPECT_TRUE(Collider::IsIntersect(box, Sphere(glm::vec3(1.5f, 0.0f, 0.0f), 0.5f)));
    EXPECT_TRUE(Collider::IsIntersect(box, Sphere(glm::vec3(1.3f, 1.3f, 0.0f), 0.5f)));
    EXPECT_FALSE(Collider::IsIntersect(box, Sphere(glm::vec3(1.4f, 1.4f, 0.0f), 0.5f)));
}

TEST(ColliderTest, BoxFrustumIntersectionHandlesInsideOutsideAndPartialOverlap)
{
    const auto frustum = MakeColliderTestFrustum();

    EXPECT_TRUE(Collider::IsIntersect(Box(glm::vec3(0.0f), glm::vec3(0.5f)), frustum));
    EXPECT_TRUE(Collider::IsIntersect(Box(glm::vec3(1.25f, 0.0f, 0.0f), glm::vec3(0.5f)), frustum));
    EXPECT_FALSE(Collider::IsIntersect(Box(glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(0.5f)), frustum));
    EXPECT_FALSE(Collider::IsIntersect(Box(glm::vec3(0.0f), glm::vec3(-0.5f)), frustum));
}

TEST(ColliderTest, SphereSphereIntersectionIncludesTangency)
{
    const Sphere sphereA(glm::vec3(0.0f), 1.0f);

    EXPECT_TRUE(Collider::IsIntersect(sphereA, Sphere(glm::vec3(2.0f, 0.0f, 0.0f), 1.0f)));
    EXPECT_TRUE(Collider::IsIntersect(sphereA, Sphere(glm::vec3(1.5f, 0.0f, 0.0f), 1.0f)));
    EXPECT_FALSE(Collider::IsIntersect(sphereA, Sphere(glm::vec3(2.01f, 0.0f, 0.0f), 1.0f)));
    EXPECT_FALSE(Collider::IsIntersect(sphereA, Sphere(glm::vec3(0.0f), -1.0f)));
}

TEST(ColliderTest, SphereFrustumIntersectionHandlesInsideOutsideAndTangency)
{
    const auto frustum = MakeColliderTestFrustum();

    EXPECT_TRUE(Collider::IsIntersect(Sphere(glm::vec3(0.0f), 0.5f), frustum));
    EXPECT_TRUE(Collider::IsIntersect(Sphere(glm::vec3(1.5f, 0.0f, 0.0f), 0.5f), frustum));
    EXPECT_FALSE(Collider::IsIntersect(Sphere(glm::vec3(1.51f, 0.0f, 0.0f), 0.5f), frustum));
    EXPECT_FALSE(Collider::IsIntersect(Sphere(glm::vec3(0.0f), 0.0f), frustum));
}
