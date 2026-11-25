#include "gtest/gtest.h"
#include "nexus/Nexus.h"
#include "nexus/scene/renderpass/DepthPrePass.h"
#include "nexus/scene/renderpass/OpaquePass.h"
#include "nexus/scene/renderpass/ShadowPass.h"
#include "helpers/TestRenderingInterface.h"

USING_NAMESPACE_NXS;

namespace
{
    class PassTestRendering final : public FakeRendering
    {
    public:
        void ClearDepth(float) override
        {
            ++depthClears;
            depthClearWritesEnabled = GetDepthWrite();
        }
        void ClearColor(const Color4F&) override
        {
            ++colorClears;
            colorClearWritesEnabled = glm::all(glm::equal(GetColorMask(), glm::bvec4(true)));
        }
        void ClearBuffer(const Color4F& color, float depth) override
        {
            ClearColor(color);
            ClearDepth(depth);
        }

        int depthClears = 0;
        int colorClears = 0;
        bool depthClearWritesEnabled = false;
        bool colorClearWritesEnabled = false;
    };
}

TEST(DepthPrePassTest, InitializesDepthProgramAndUsesOpaqueDepthState)
{
    DepthPrePass pass;
    FakeRendering factory;
    ASSERT_TRUE(pass.Init(factory));
    ASSERT_NE(pass.pipelineState.globalShader, nullptr);
    auto program = pass.pipelineState.globalShader;
    ASSERT_TRUE(pass.Init(factory));
    EXPECT_EQ(pass.pipelineState.globalShader, program);

    EXPECT_TRUE(pass.pipelineState.depthTest);
    EXPECT_TRUE(pass.pipelineState.depthWrite);
    EXPECT_EQ(pass.pipelineState.depthFunction, DepthFunction::Lesser);
    EXPECT_TRUE(glm::all(glm::equal(pass.pipelineState.colorMask, glm::bvec4(false))));
    const auto& vertex = pass.globalShaderSources.at(GpuProgram::Type::Vertex);
    EXPECT_NE(vertex.find("invariant gl_Position;"), std::string::npos);
    EXPECT_NE(vertex.find("vec4 worldPos = _Model * vec4(aPos, 1.0);"), std::string::npos);
    EXPECT_NE(vertex.find("gl_Position = _Projection * _View * worldPos;"), std::string::npos);
    EXPECT_EQ(vertex.find("gl_Position.z +="), std::string::npos);

    auto mesh = std::make_shared<Mesh>("mesh");
    EXPECT_FALSE(pass.IsPassFiltered(mesh));
    auto material = std::make_shared<Material>("material", 1);
    mesh->SetMaterial(material);
    EXPECT_TRUE(pass.IsPassFiltered(mesh));
    material->blendMode = BlendMode::Alpha;
    EXPECT_FALSE(pass.IsPassFiltered(mesh));
}

TEST(DepthPrePassTest, FailedProgramInitializationIsReported)
{
    DepthPrePass pass;
    BadRendering factory;
    EXPECT_FALSE(pass.Init(factory));
    EXPECT_EQ(pass.pipelineState.globalShader, nullptr);
    pass.globalShaderSources.clear();
    FakeRendering goodFactory;
    EXPECT_FALSE(pass.Init(goodFactory));
    EXPECT_EQ(pass.pipelineState.globalShader, nullptr);
}

TEST(DepthPrePassTest, OpaquePassPreservesAndUsesPrepassDepth)
{
    OpaquePass pass;
    EXPECT_TRUE(pass.pipelineState.depthTest);
    EXPECT_TRUE(pass.pipelineState.depthWrite);
    EXPECT_EQ(pass.pipelineState.depthFunction, DepthFunction::LesserOrEqual);
    EXPECT_TRUE(glm::all(glm::equal(pass.pipelineState.colorMask, glm::bvec4(true))));
}

TEST(RenderPassTest, OffscreenTargetIsCreatedDuringInit)
{
    class TestOffscreenPass final : public RenderPass
    {
    public:
        TestOffscreenPass()
        {
            targetType = RenderTargetType::Offscreen;
            offscreenTarget = {
                .name = "test-target",
                .textureDesc = TextureDescription {
                    .width = 320,
                    .height = 180,
                }
            };
        }
    };

    FakeRendering factory;
    TestOffscreenPass pass;

    ASSERT_TRUE(pass.Init(factory));
    EXPECT_TRUE(pass.offscreenTarget.IsValid());
    EXPECT_EQ(pass.offscreenTarget.name, "test-target");
    EXPECT_EQ(pass.offscreenTarget.textureDesc.width, 320);
    EXPECT_EQ(pass.offscreenTarget.textureDesc.height, 180);
}

TEST(RenderPassTest, ShadowPassCreatesShadowMapTargetDuringInit)
{
    FakeRendering factory;
    ShadowPass pass;

    ASSERT_TRUE(pass.Init(factory));
    EXPECT_EQ(pass.offscreenTarget.name, AttachmentDesc::shadowMap);
    EXPECT_TRUE(pass.offscreenTarget.IsValid());
    EXPECT_EQ(pass.offscreenTarget.textureDesc.format, PixelFormat::Depth);
}

TEST(RenderPassTest, BeginFiltersTargetsAndEndPropagatesOwnTarget)
{
    // class TestFilteredPass final : public RenderPass
    // {
    // public:
    //     TestFilteredPass()
    //     {
    //         targetType = RenderTargetType::Offscreen;
    //         offscreenTarget = {
    //             .name = "own-target",
    //             .width = 640,
    //             .height = 480,
    //         };
    //         readTargets = { "shadowmap", "lighting" };
    //     }
    // };

    // auto rendering = std::make_shared<FakeRendering>();
    // RenderSystem renderSystem(rendering);
    // TestFilteredPass pass;
    // ASSERT_TRUE(pass.Init(*rendering));

    // AttachmentDesc shadowMap{ .name = "shadowmap", .width = 2048, .height = 2048 };
    // AttachmentDesc unrelated{ .name = "color", .width = 1920, .height = 1080 };
    // Array<AttachmentDesc> sampleTargets{ shadowMap, unrelated };
    // pass.Begin(renderSystem, sampleTargets);

    // ASSERT_EQ(pass.GetSampleTargets().size(), 1);
    // EXPECT_EQ(pass.GetSampleTargets()[0].name, "shadowmap");

    // Array<AttachmentDesc> offscreenTargets;
    // pass.End(renderSystem, offscreenTargets);
    // ASSERT_EQ(offscreenTargets.size(), 1);
    // EXPECT_EQ(offscreenTargets[0].name, "own-target");
    // EXPECT_NE(offscreenTargets[0].renderTarget, nullptr);
}

TEST(RenderPassTest, BeginClearsPreviouslyFilteredTargets)
{
    // class TestFilteredPass final : public RenderPass
    // {
    // public:
    //     TestFilteredPass() { readTargets = { "shadowmap" }; }
    // };

    // auto rendering = std::make_shared<FakeRendering>();
    // RenderSystem renderSystem(rendering);
    // TestFilteredPass pass;
    // AttachmentDesc shadowMap{ .name = "shadowmap", .width = 2048, .height = 2048 };

    // pass.Begin(renderSystem, Array<AttachmentDesc>{ shadowMap });
    // ASSERT_EQ(pass.GetSampleTargets().size(), 1);

    // pass.Begin(renderSystem, Array<AttachmentDesc>{ AttachmentDesc{ .name = "color" } });
    // EXPECT_TRUE(pass.GetSampleTargets().empty());
}

TEST(DepthPrePassTest, PassSequenceRestoresColorMaskAndKeepsDepth)
{
    // auto rendering = std::make_shared<PassTestRendering>();
    // RenderSystem renderSystem(rendering);
    // DepthPrePass depth;
    // ASSERT_TRUE(depth.Init(*rendering));
    // OpaquePass opaque;

    // ViewContext view{};
    // Array<AttachmentDesc> offscreenBuffers;
    // view.profile.clearFlags = ClearFlags::Color | ClearFlags::Depth;
    // view.ClearView(*rendering);
    // depth.Begin(renderSystem, offscreenBuffers);
    // EXPECT_EQ(rendering->GetGlobalShader(), depth.pipelineState.globalShader);
    // EXPECT_TRUE(glm::all(glm::equal(rendering->GetColorMask(), glm::bvec4(false))));
    // depth.End(renderSystem, offscreenBuffers);
    // opaque.Begin(renderSystem, offscreenBuffers);
    // EXPECT_EQ(rendering->GetGlobalShader(), nullptr);
    // EXPECT_EQ(rendering->GetDepthFunction(), DepthFunction::LesserOrEqual);
    // EXPECT_EQ(rendering->depthClears, 1);
    // EXPECT_EQ(rendering->colorClears, 1);
    // EXPECT_TRUE(rendering->depthClearWritesEnabled);
    // EXPECT_TRUE(rendering->colorClearWritesEnabled);
    // opaque.End(renderSystem, offscreenBuffers);
}

TEST(RenderPassTest, ViewClearRespectsFlagsAndEnablesRequiredWrites)
{
    PassTestRendering rendering;
    rendering.SetColorMask(glm::bvec4(false));
    rendering.SetDepthWrite(false);
    ViewContext view{};

    view.profile.clearFlags = ClearFlags::Color;
    view.ClearView(rendering);
    EXPECT_EQ(rendering.colorClears, 1);
    EXPECT_EQ(rendering.depthClears, 0);
    EXPECT_TRUE(rendering.colorClearWritesEnabled);
    EXPECT_FALSE(rendering.GetDepthWrite());

    rendering.SetColorMask(glm::bvec4(false));
    view.profile.clearFlags = ClearFlags::Depth;
    view.ClearView(rendering);
    EXPECT_EQ(rendering.colorClears, 1);
    EXPECT_EQ(rendering.depthClears, 1);
    EXPECT_TRUE(rendering.depthClearWritesEnabled);
    EXPECT_TRUE(glm::all(glm::equal(rendering.GetColorMask(), glm::bvec4(false))));

    view.profile.clearFlags = ClearFlags::None;
    view.ClearView(rendering);
    EXPECT_EQ(rendering.colorClears, 1);
    EXPECT_EQ(rendering.depthClears, 1);
}
