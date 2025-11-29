#include "nexus/Nexus.h"
#include "gtest/gtest.h"

using namespace nxs;

TEST(SceneRenderer, RegisterRenderPass_SortsByPriority)
{
    ForwardSceneRenderer renderer(*Engine::Instance().GetRenderSystem());

    RenderPass p1; p1.SetName("p1").SetPriority(100);
    RenderPass p2; p2.SetName("p2").SetPriority(10);
    RenderPass p3; p3.SetName("p3").SetPriority(50);

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
