#include "nexus/Nexus.h"
#include "gtest/gtest.h"

#include "helpers/TestRenderingInterface.h"

using namespace nxs;

class CountingSceneView final : public SceneNode, public ISceneView
{
public:
    explicit CountingSceneView(Ref<entt::registry> registry, std::string name)
        : SceneNode(std::move(registry), std::move(name))
    {
    }

    NODISCARD glm::mat4 GetViewMatrix() const override { return glm::mat4(1.0f); }
    NODISCARD glm::mat4 GetProjectionMatrix() const override { return glm::mat4(1.0f); }
    NODISCARD glm::mat4 GetViewProjectionMatrix() const override { return glm::mat4(1.0f); }

    NODISCARD Array<SceneNode*> GetVisibleNodes(const Scene&) const override
    {
        ++visibilityCalls;
        return {};
    }

    mutable uint32_t visibilityCalls = 0;

private:
    glm::vec3 m_position{0.0f};
};

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