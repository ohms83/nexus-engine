#include "gtest/gtest.h"
#include "nexus/Nexus.h"

USING_NAMESPACE_NXS;

TEST(RenderPassTest, SerializeDeserialize)
{
    RenderPass pass;
    pass.name = "TestPass";
    pass.priority = 123;
    pass.enabled = true;
    pass.targetType = RenderTargetType::Offscreen;
    pass.clearFlags = ClearFlags::Color | ClearFlags::Depth;
    pass.clearColor = Color3F{0.2f, 0.3f, 0.4f};
    pass.clearDepth = 0.5f;
    pass.layerMask = 42;
    pass.filterType = "opaque";

    pass.pipelineState.depthTest = true;
    pass.pipelineState.depthWrite = false;
    pass.pipelineState.depthFunction = DepthFunction::Greater;
    pass.pipelineState.polygonMode = PolygonMode::Wireframe;

    VariantData serialized = pass.Serialize();

    RenderPass pass2;
    pass2.Deserialize(serialized);

    EXPECT_EQ(pass2.name, pass.name);
    EXPECT_EQ(pass2.priority, pass.priority);
    EXPECT_EQ(pass2.enabled, pass.enabled);
    EXPECT_EQ(pass2.targetType, pass.targetType);
    EXPECT_EQ(pass2.clearFlags, pass.clearFlags);
    EXPECT_FLOAT_EQ(pass2.clearColor.r, pass.clearColor.r);
    EXPECT_FLOAT_EQ(pass2.clearDepth, pass.clearDepth);
    EXPECT_EQ(pass2.layerMask, pass.layerMask);
    EXPECT_EQ(pass2.pipelineState.depthTest, pass.pipelineState.depthTest);
    EXPECT_EQ(pass2.pipelineState.depthFunction, pass.pipelineState.depthFunction);
    EXPECT_EQ(pass2.pipelineState.polygonMode, pass.pipelineState.polygonMode);
}

TEST(RenderPassTest, FilterPreset)
{
    RenderPass pass;
    pass.filterType = "alpha";
    pass.Deserialize(pass.Serialize());
    // The filter callback should be set to alpha preset (blendMode != None)
    Material mOpaque("mat", 1);
    mOpaque.blendMode = BlendMode::None;
    Material mAlpha("matA", 2);
    mAlpha.blendMode = BlendMode::Alpha;

    EXPECT_FALSE(pass.MatchesMaterial(mOpaque));
    EXPECT_TRUE(pass.MatchesMaterial(mAlpha));
}
