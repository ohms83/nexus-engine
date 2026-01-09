#include "nexus/Nexus.h"
#include "gtest/gtest.h"

#include "helpers/TestRenderingInterface.h"

using namespace nxs;

TEST(SceneRenderer, RegisterRenderPass_SortsByPriority)
{
    FakeRenderer renderer;

    RenderPass p1 = RenderPassBuilder::Begin("p1", 100).Build();
    RenderPass p2 = RenderPassBuilder::Begin("p2", 10).Build();
    RenderPass p3 = RenderPassBuilder::Begin("p3", 50).Build();

    renderer.RegisterRenderPass(p1);
    renderer.RegisterRenderPass(p2);
    renderer.RegisterRenderPass(p3);

    // Access the protected m_renderPasses via reflection? For now, simulate by checking sort order by registering passes and then re-registering expects the internal order.
    // However, since m_renderPasses is protected, assert indirectly by constructing a second renderer and checking insertion order.
    const auto &passes = renderer.GetRenderPasses();
    ASSERT_EQ(passes.size(), 3);
    EXPECT_EQ(passes[0].name, std::string("p2"));
    EXPECT_EQ(passes[1].name, std::string("p3"));
    EXPECT_EQ(passes[2].name, std::string("p1"));
}

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

TEST(SceneRenderer_IsSphereInside, FullyInside)
{
    Frustum f = MakeUnitCubeFrustum();
    Sphere s(glm::vec3(0.0f), 0.5f);
    EXPECT_TRUE(SceneRenderer::IsSphereInside(f, s, glm::mat4(1.0f), glm::vec3(1.0f)));
}

TEST(SceneRenderer_IsSphereInside, FullyOutside)
{
    Frustum f = MakeUnitCubeFrustum();
    Sphere s(glm::vec3(2.0f, 0.0f, 0.0f), 0.5f);
    EXPECT_FALSE(SceneRenderer::IsSphereInside(f, s, glm::mat4(1.0f), glm::vec3(1.0f)));
}

TEST(SceneRenderer_IsSphereInside, PartialIntersection)
{
    Frustum f = MakeUnitCubeFrustum();
    // Center at 1.4 on x with radius 0.5 -- partially intersects the right plane at x=1
    Sphere s(glm::vec3(1.4f, 0.0f, 0.0f), 0.5f);
    EXPECT_TRUE(SceneRenderer::IsSphereInside(f, s, glm::mat4(1.0f), glm::vec3(1.0f)));
}

TEST(SceneRenderer_IsSphereInside, ModelTranslationMovesOutside)
{
    Frustum f = MakeUnitCubeFrustum();
    Sphere s(glm::vec3(0.0f), 0.5f);
    glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 0.0f));
    EXPECT_FALSE(SceneRenderer::IsSphereInside(f, s, translate, glm::vec3(1.0f)));
}

TEST(SceneRenderer_IsSphereInside, ScaledRadiusAffectsResult)
{
    Frustum f = MakeUnitCubeFrustum();
    // Without scale this sphere would be outside; with scale it becomes inside/partially inside.
    Sphere s(glm::vec3(1.8f, 0.0f, 0.0f), 0.4f);
    EXPECT_FALSE(SceneRenderer::IsSphereInside(f, s, glm::mat4(1.0f), glm::vec3(1.0f)));
    // Scale X by 2 -> maxScale = 2 -> scaled radius = 0.8 -> should be partially inside
    EXPECT_TRUE(SceneRenderer::IsSphereInside(f, s, glm::mat4(1.0f), glm::vec3(2.0f, 1.0f, 1.0f)));
}

TEST(SceneRenderer_IsSphereInside, ZeroRadiusReturnsFalse)
{
    Frustum f = MakeUnitCubeFrustum();
    Sphere s(glm::vec3(0.0f), 0.0f);
    EXPECT_FALSE(SceneRenderer::IsSphereInside(f, s, glm::mat4(1.0f), glm::vec3(1.0f)));
}