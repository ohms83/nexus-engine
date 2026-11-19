#include "gtest/gtest.h"
#include "nexus/Nexus.h"
#include "nexus/scene/renderpass/DepthPrePass.h"
#include "nexus/scene/renderpass/OpaquePass.h"
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

TEST(DepthPrePassTest, PassSequenceRestoresColorMaskAndKeepsDepth)
{
    auto rendering = std::make_shared<PassTestRendering>();
    RenderSystem renderSystem(rendering);
    DepthPrePass depth;
    ASSERT_TRUE(depth.Init(*rendering));
    OpaquePass opaque;

    ViewContext view{};
    view.profile.clearFlags = ClearFlags::Color | ClearFlags::Depth;
    view.ClearView(*rendering);
    depth.Begin(renderSystem);
    EXPECT_EQ(rendering->GetGlobalShader(), depth.pipelineState.globalShader);
    EXPECT_TRUE(glm::all(glm::equal(rendering->GetColorMask(), glm::bvec4(false))));
    depth.End(renderSystem);
    opaque.Begin(renderSystem);
    EXPECT_EQ(rendering->GetGlobalShader(), nullptr);
    EXPECT_EQ(rendering->GetDepthFunction(), DepthFunction::LesserOrEqual);
    EXPECT_EQ(rendering->depthClears, 1);
    EXPECT_EQ(rendering->colorClears, 1);
    EXPECT_TRUE(rendering->depthClearWritesEnabled);
    EXPECT_TRUE(rendering->colorClearWritesEnabled);
    opaque.End(renderSystem);
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

TEST(RenderPassTest, SerializeDeserialize)
{
    RenderPass pass;
    pass.name = "TestPass";
    pass.priority = 123;
    pass.enabled = true;
    pass.targetType = RenderTargetType::Offscreen;
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
    Ref<Material> mOpaque = std::make_shared<Material>("mat", 1);
    mOpaque->blendMode = BlendMode::None;
    RenderCommand cmdOpaque;
    cmdOpaque.material = mOpaque;

    Ref<Material> mAlpha = std::make_shared<Material>("matA", 2);
    mAlpha->blendMode = BlendMode::Alpha;
    RenderCommand cmdAlpha;
    cmdAlpha.material = mAlpha;

    EXPECT_FALSE(pass.IsPassFiltered(cmdOpaque));
    EXPECT_TRUE(pass.IsPassFiltered(cmdAlpha));
}
