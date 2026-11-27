#include "nexus/Nexus.h"
#include "gtest/gtest.h"
#include "nexus/scene/renderpass/ShadowPass.h"

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
    NODISCARD Ref<RenderTarget> GetRenderTarget() const override { return nullptr; }
    void SetRenderTarget(Ref<RenderTarget>) override {}

    NODISCARD ViewContext CreateViewContext(const Scene&) const override
    {
        ++visibilityCalls;
        ViewContext context{};
        context.view = this;
        context.profile = profile;
        context.profile.clearFlags = ClearFlags::Color | ClearFlags::Depth;
        context.viewMatrix = context.projectionMatrix = context.viewProjectionMatrix = glm::mat4(1.0f);
        context.visibleNodes = visibleNodes;
        return context;
    }

    mutable uint32_t visibilityCalls = 0;
    RenderProfile profile;
    Array<SceneNode*> visibleNodes;
};

namespace
{
    class FailedInitPass final : public RenderPass
    {
    public:
        bool Init(const IGpuResourceFactory& factory) override { return false; }
    };

    class CountingRenderer final : public SceneRenderer
    {
    public:
        explicit CountingRenderer(bool fails)
        {
            if (fails) RegisterRenderPass(RenderPassQueue::Opaque, NewRef<FailedInitPass>());
        }

        void Render(RenderSystem&, const Scene&) override { ++renders; }
        int renders = 0;
    };

    class ShadowTestProgram final : public FakeGpuProgram
    {
    public:
        void Bind() override {}
        void Unbind() override {}
        bool SetUniformInt(const std::string& name, int32_t value) override
        {
            ints[name] = value;
            return true;
        }
        bool SetUniformFloat(const std::string&, float) override { return true; }
        bool SetUniformVector(const std::string&, const glm::vec3&) override { return true; }
        bool SetUniformMatrix(const std::string& name, const glm::mat4& value, bool) override
        {
            matrices[name] = value;
            return true;
        }
        bool SetUniformTexture2D(const std::string& name, Ref<const TextureProxy>, int32_t unit) override
        {
            textureUnits[name] = unit;
            return true;
        }

        std::unordered_map<std::string, int32_t> ints;
        std::unordered_map<std::string, glm::mat4> matrices;
        std::unordered_map<std::string, int32_t> textureUnits;
    };

    class ShadowTestFactory final : public FakeGpuResourceFactory
    {
    public:
        Ref<GpuProgram> CreateGpuProgram() const override
        {
            program = NewRef<ShadowTestProgram>();
            return program;
        }
        mutable Ref<ShadowTestProgram> program;
    };

    class ShadowTestRendering final : public FakeRendering
    {
    public:
        void ClearDepth(float) override {}
        void DrawIndexed(const Ref<IndexBuffer>) override { ++draws; }
        int draws = 0;
    };

    class LightParamsTestPass final : public RenderPass
    {
    public:
        using RenderPass::SetDirectLightParams;
        using RenderPass::SetShadowMapParams;
    };

    class ViewTestProgram final : public GpuProgram
    {
    public:
        void Bind() override {}
        bool IsBinding() const override { return false; }
        bool SetUniformInt(const std::string&, int32_t) override { return true; }
        bool SetUniformFloat(const std::string&, float) override { return true; }
        bool SetUniformVector(const std::string&, const glm::vec3&) override { return true; }
        bool SetUniformMatrix(const std::string&, const glm::mat4&, bool) override { return true; }

    private:
        int32_t FindUniform_Internal(const std::string&) const override { return -1; }
    };

    class ViewTestRendering final : public FakeRendering
    {
    public:
        void ClearBuffer(const Color4F&, float) override
        {
            ++clears;
            clearMasksValid &= GetDepthWrite() && glm::all(glm::equal(GetColorMask(), glm::bvec4(true)));
        }
        void DrawIndexed(const Ref<IndexBuffer>) override
        {
            drawMasks.push_back(GetColorMask());
        }
        int clears = 0;
        bool clearMasksValid = true;
        Array<glm::bvec4> drawMasks;
    };

    class ViewTestPass final : public RenderPass
    {
    public:
        bool IsPassFiltered(Ref<const Mesh> mesh) const override
        {
            return mesh->GetMaterial()->blendMode == BlendMode::None;
        }
        void AppendCommands(RenderSystem&, const Array<SceneNode*>&, Array<RenderCommand>& commands) const override
        {
            ++appendCalls;
            if (appendedMesh)
            {
                RenderCommand command{};
                command.material = appendedMesh->GetMaterial();
                command.vertexBuffer = appendedMesh->GetVertexBuffer();
                command.indexBuffer = appendedMesh->GetIndexBuffer();
                command.modelMatrix = glm::mat4x3(1.0f);
                commands.push_back(command);
            }
        }
        Ref<Mesh> appendedMesh;
        mutable int appendCalls = 0;
    };

    Ref<Mesh> MakeShadowTestMesh(const Ref<Material>& material)
    {
        auto mesh = NewRef<Mesh>("test");
        mesh->SetMaterial(material);
        mesh->SetSphere(Sphere(glm::vec3(0.0f), 1.0f));
        mesh->SetVertexBuffer(NewRef<FakdeVertexBuffer>());
        mesh->SetIndexBuffer(NewRef<FakeIndexBuffer>());
        return mesh;
    }
}

TEST(RenderSystemTest, UsesRenderingInterfaceConfigAndTracksResize)
{
    class ConfiguredRendering final : public FakeRendering
    {
    public:
        ConfiguredRendering()
        {
            m_config.api = GraphicsAPI::OpenGL;
            m_config.screenWidth = 1280;
            m_config.screenHeight = 720;
            m_config.vsync = true;
        }

        void OnResize(uint32_t, uint32_t) override {}
    };

    auto rendering = NewRef<ConfiguredRendering>();
    FakeGpuResourceFactory factory;
    RenderSystem renderSystem(rendering, &factory);
    EXPECT_EQ(renderSystem.GetRenderInterface(), rendering);
    EXPECT_EQ(renderSystem.GetGraphicsConfig().api, GraphicsAPI::OpenGL);
    EXPECT_EQ(renderSystem.GetGraphicsConfig().screenWidth, 1280);
    EXPECT_EQ(renderSystem.GetGraphicsConfig().screenHeight, 720);
    EXPECT_TRUE(renderSystem.GetGraphicsConfig().vsync);

    renderSystem.OnResize(640, 480);
    EXPECT_EQ(renderSystem.GetGraphicsConfig().screenWidth, 640);
    EXPECT_EQ(renderSystem.GetGraphicsConfig().screenHeight, 480);
    EXPECT_TRUE(renderSystem.GetGraphicsConfig().vsync);
}

TEST(SceneRendererTest, ExecutesSelectedPassesPerViewWithFreshCommandsAndClearMasks)
{
    auto scene = NewRef<Scene>("scene");
    auto first = scene->EmplaceChild<CountingSceneView>("first");
    auto second = scene->EmplaceChild<CountingSceneView>("second");
    auto inactive = scene->EmplaceChild<CountingSceneView>("inactive");
    inactive->Activate(false);
    auto otherScene = NewRef<Scene>("other");
    auto foreign = otherScene->EmplaceChild<CountingSceneView>("foreign");
    auto material = NewRef<Material>("opaque", 1);
    auto mesh = scene->EmplaceChild<SceneNode>("mesh");
    mesh->AddComponent<MeshComponent>()->SetMesh(MakeShadowTestMesh(material));
    first->visibleNodes.push_back(mesh.get());
    second->visibleNodes.push_back(mesh.get());

    auto depth = NewRef<ViewTestPass>();
    auto opaque = NewRef<ViewTestPass>();
    first->profile.depthFill = true;
    second->profile.depthFill = true;
    second->profile.culling = CullingFlags::Opaque;
    depth->pipelineState.colorMask = glm::bvec4(false);
    depth->pipelineState.depthWrite = false;
    opaque->pipelineState.colorMask = glm::bvec4(true);
    depth->pipelineState.globalShader = NewRef<ViewTestProgram>();
    opaque->pipelineState.globalShader = NewRef<ViewTestProgram>();
    int depthBegins = 0, depthEnds = 0, opaqueBegins = 0, opaqueEnds = 0;
    depth->onBegin = [&](RenderSystem&, SceneRenderingContext&) { ++depthBegins; };
    depth->onEnd = [&](RenderSystem&, SceneRenderingContext&) { ++depthEnds; };
    opaque->onBegin = [&](RenderSystem&, SceneRenderingContext&) { ++opaqueBegins; };
    opaque->onEnd = [&](RenderSystem&, SceneRenderingContext&) { ++opaqueEnds; };

    auto rendering = NewRef<ViewTestRendering>();
    FakeGpuResourceFactory factory;
    RenderSystem renderSystem(rendering, &factory);
    SceneRenderer renderer;
    renderer.RegisterRenderPass(RenderPassQueue::DepthFill, depth);
    renderer.RegisterRenderPass(RenderPassQueue::Opaque, opaque);
    renderer.Init(&factory, renderSystem);
    renderer.Render(renderSystem, *scene);
    renderer.Render(renderSystem, *scene);

    EXPECT_EQ(first->visibilityCalls, 2u);
    EXPECT_EQ(second->visibilityCalls, 2u);
    EXPECT_EQ(inactive->visibilityCalls, 0u);
    EXPECT_EQ(foreign->visibilityCalls, 0u);
    EXPECT_EQ(rendering->clears, 4);
    EXPECT_TRUE(rendering->clearMasksValid);
    EXPECT_EQ(depthBegins, 4);
    EXPECT_EQ(depthEnds, 4);
    EXPECT_EQ(opaqueBegins, 2);
    EXPECT_EQ(opaqueEnds, 2);
    ASSERT_EQ(rendering->drawMasks.size(), 6u);
    for (size_t i = 0; i < rendering->drawMasks.size(); ++i)
        EXPECT_TRUE(glm::all(glm::equal(rendering->drawMasks[i], glm::bvec4(i % 3 == 1))));
}

TEST(SceneRendererTest, EmptyViewStillRunsPassLifecycle)
{
    auto scene = NewRef<Scene>("scene");
    auto view = scene->EmplaceChild<CountingSceneView>("empty");
    auto pass = NewRef<ViewTestPass>();
    pass->appendedMesh = MakeShadowTestMesh(NewRef<Material>("overlay", 1));
    pass->pipelineState.globalShader = NewRef<ViewTestProgram>();
    int begins = 0, ends = 0;
    pass->onBegin = [&](RenderSystem&, SceneRenderingContext&) { ++begins; };
    pass->onEnd = [&](RenderSystem&, SceneRenderingContext&) { ++ends; };
    SceneRenderer renderer;
    renderer.RegisterRenderPass(RenderPassQueue::Opaque, pass);
    auto rendering = NewRef<ViewTestRendering>();
    FakeGpuResourceFactory factory;
    RenderSystem renderSystem(rendering, &factory);
    renderer.Init(&factory, renderSystem);
    renderer.Render(renderSystem, *scene);
    EXPECT_EQ(begins, 1);
    EXPECT_EQ(ends, 1);
    EXPECT_EQ(pass->appendCalls, 1);
    EXPECT_EQ(rendering->drawMasks.size(), 1u);
}

TEST(SceneRendererTest, DisabledPassDoesNotRun)
{
    auto scene = NewRef<Scene>("scene");
    scene->EmplaceChild<CountingSceneView>("view");
    auto pass = NewRef<ViewTestPass>();
    pass->enabled = false;
    SceneRenderer renderer;
    renderer.RegisterRenderPass(RenderPassQueue::Opaque, pass);
    auto rendering = NewRef<ViewTestRendering>();
    FakeGpuResourceFactory factory;
    RenderSystem renderSystem(rendering, &factory);

    renderer.Init(&factory, renderSystem);
    renderer.Render(renderSystem, *scene);

    EXPECT_EQ(pass->appendCalls, 0);
    EXPECT_TRUE(rendering->drawMasks.empty());
}

TEST(SceneRendererTest, RenderProfileSelectsPassesInQueueOrder)
{
    auto scene = NewRef<Scene>("scene");
    auto view = scene->EmplaceChild<CountingSceneView>("view");
    SceneRenderer renderer;
    Array<RenderPassQueue> executed;
    auto registerPass = [&](RenderPassQueue queue) {
        auto pass = NewRef<ViewTestPass>();
        pass->onBegin = [&, queue](RenderSystem&, SceneRenderingContext&) { executed.push_back(queue); };
        renderer.RegisterRenderPass(queue, pass);
        return pass;
    };
    auto overlay = registerPass(RenderPassQueue::Overlay);
    auto alpha = registerPass(RenderPassQueue::Alpha);
    registerPass(RenderPassQueue::Opaque);
    registerPass(RenderPassQueue::DepthFill);
    auto rendering = NewRef<ViewTestRendering>();
    FakeGpuResourceFactory factory;
    RenderSystem renderSystem(rendering, &factory);

    view->profile.depthFill = true;
    renderer.Init(&factory, renderSystem);
    renderer.Render(renderSystem, *scene);
    EXPECT_EQ(executed, (Array<RenderPassQueue>{
        RenderPassQueue::DepthFill, RenderPassQueue::Opaque,
        RenderPassQueue::Alpha, RenderPassQueue::Overlay}));

    executed.clear();
    view->profile.depthFill = false;
    view->profile.culling = static_cast<CullingFlags>(
        static_cast<uint32_t>(CullingFlags::Opaque) | static_cast<uint32_t>(CullingFlags::Alpha));
    renderer.Render(renderSystem, *scene);
    EXPECT_EQ(executed, (Array<RenderPassQueue>{RenderPassQueue::Overlay}));

    executed.clear();
    view->profile.culling = CullingFlags::None;
    alpha->enabled = false;
    overlay->enabled = false;
    renderer.Render(renderSystem, *scene);
    EXPECT_EQ(executed, (Array<RenderPassQueue>{RenderPassQueue::Opaque}));
}

TEST(SceneRendererTest, RendersVisibleMeshThroughSceneCamera)
{
    auto scene = NewRef<Scene>("scene");
    auto camera = scene->EmplaceChild<Camera>("camera");
    camera->SetPosition({0.0f, 0.0f, 5.0f});
    camera->LookAt(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    camera->SetProjection(60.0f, 800.0f, 600.0f, 0.1f, 100.0f);
    auto mesh = scene->EmplaceChild<SceneNode>("mesh");
    mesh->AddComponent<MeshComponent>()->SetMesh(
        MakeShadowTestMesh(NewRef<Material>("opaque", 1)));

    auto pass = NewRef<ViewTestPass>();
    pass->pipelineState.globalShader = NewRef<ViewTestProgram>();
    auto rendering = NewRef<ViewTestRendering>();
    FakeGpuResourceFactory factory;
    RenderSystem renderSystem(rendering, &factory);
    SceneRenderer renderer;
    renderer.Init(&factory, renderSystem);
    renderer.RegisterRenderPass(RenderPassQueue::Opaque, pass);
    renderer.Render(renderSystem, *scene);

    EXPECT_EQ(rendering->clears, 1);
    EXPECT_EQ(rendering->drawMasks.size(), 1u);
}

TEST(SceneRendererTest, ViewportResizeConnectionFollowsRendererLifetime)
{
    auto rendering = NewRef<FakeRendering>();
    FakeGpuResourceFactory factory;
    RenderSystem renderSystem(rendering, &factory);

    {
        CountingRenderer renderer(false);
        ASSERT_TRUE(renderer.Init(&factory, renderSystem));
        EXPECT_EQ(renderSystem.onViewportResize.slot_count(), 1u);
        ASSERT_TRUE(renderer.Init(&factory, renderSystem));
        EXPECT_EQ(renderSystem.onViewportResize.slot_count(), 1u);
        renderSystem.onViewportResize(640u, 480u);
    }

    EXPECT_EQ(renderSystem.onViewportResize.slot_count(), 0u);

    CountingRenderer failing(true);
    EXPECT_FALSE(failing.Init(&factory, renderSystem));
    EXPECT_EQ(renderSystem.onViewportResize.slot_count(), 0u);
}

TEST(SceneRendererTest, FailedRendererInitializationKeepsPreviousRenderer)
{
    auto scene = NewRef<Scene>("scene");
    auto rendering = NewRef<FakeRendering>();
    FakeGpuResourceFactory factory;
    RenderSystem renderSystem(rendering, &factory);
    GraphicsConfig fakeConfig {};
    auto previous = NewRef<CountingRenderer>(false);
    auto failing = NewRef<CountingRenderer>(true);
    previous->Init(&factory, renderSystem);
    failing->Init(&factory, renderSystem);

    scene->SetRenderer(previous);
    scene->SetRenderer(failing);
    scene->Render(renderSystem);
    EXPECT_EQ(previous->renders, 1);
    EXPECT_EQ(failing->renders, 0);

    scene->SetRenderer(nullptr);
    scene->Render(renderSystem);
    EXPECT_EQ(previous->renders, 1);
}

TEST(SceneRendererTest, InactiveAncestorExcludesCameraAndLight)
{
    auto scene = NewRef<Scene>("scene");
    auto parent = scene->EmplaceChild<SceneNode>("parent");
    parent->EmplaceChild<Camera>("camera");
    parent->EmplaceChild<DirectionalLight>("light");
    auto pass = NewRef<ViewTestPass>();
    auto rendering = NewRef<ViewTestRendering>();
    FakeGpuResourceFactory factory;
    RenderSystem renderSystem(rendering, &factory);
    SceneRenderer viewRenderer;
    viewRenderer.RegisterRenderPass(RenderPassQueue::Opaque, pass);
    auto program = NewRef<ShadowTestProgram>();
    LightParamsTestPass lightParamsPass;

    viewRenderer.Init(&factory, renderSystem);
    viewRenderer.Render(renderSystem, *scene);
    EXPECT_EQ(pass->appendCalls, 1);
    lightParamsPass.SetDirectLightParams(*program, *scene);
    EXPECT_EQ(program->ints["_NumDirectLight"], 1);

    parent->Activate(false);
    viewRenderer.Render(renderSystem, *scene);
    EXPECT_EQ(pass->appendCalls, 1);
    lightParamsPass.SetDirectLightParams(*program, *scene);
    EXPECT_EQ(program->ints["_NumDirectLight"], 0);
}

TEST(SceneRendererShadowTest, SelectsEnabledLightAndUpdatesPassParameters)
{
    auto scene = NewRef<Scene>("shadow");
    auto disabledLight = scene->EmplaceChild<DirectionalLight>("disabled");
    disabledLight->Direction() = glm::vec3(0.0f, -1.0f, 0.0f);
    disabledLight->Properties().canCastShadows = false;
    auto light = scene->EmplaceChild<DirectionalLight>("enabled");
    light->Direction() = glm::vec3(0.0f, -1.0f, 0.0f);

    auto material = NewRef<Material>("shadow", 1);
    material->castShadow = true;
    auto caster = scene->EmplaceChild<SceneNode>("caster");
    caster->AddComponent<MeshComponent>()->SetMesh(MakeShadowTestMesh(material));

    auto rendering = NewRef<ShadowTestRendering>();
    ShadowTestFactory factory;
    RenderSystem renderSystem(rendering, &factory);
    ShadowPass pass;
    ASSERT_TRUE(pass.Init(factory));
    SceneRenderingContext context{ .scene = scene.get() };

    pass.Begin(renderSystem, context);
    pass.Render(renderSystem, context);
    pass.End(renderSystem, context);

    EXPECT_EQ(rendering->draws, 1);
    ASSERT_EQ(context.shadowMaps.size(), 1u);
    EXPECT_NE(context.shadowMaps.front().depthMap, nullptr);
    ASSERT_NE(factory.program, nullptr);
    EXPECT_TRUE(factory.program->matrices.contains("_LightSpaceMatrix"));
    EXPECT_TRUE(factory.program->matrices.contains("_Model"));

    LightParamsTestPass lightParamsPass;
    lightParamsPass.SetDirectLightParams(
        *factory.program,
        *scene,
        context.shadowMaps.front().lightId);
    lightParamsPass.SetShadowMapParams(*factory.program, context);
    int32_t expectedShadowLightIndex = 0;
    for (const auto view = scene->GetRegistry()->view<const SceneNodeComponent, const DirectLightComponent>();
         const auto& [entity, nodeComponent, directLight] : view.each())
    {
        const auto* node = SceneNode::FindNode(nodeComponent.id);
        if (!node || !node->IsActive()) continue;
        if (nodeComponent.id == context.shadowMaps.front().lightId) break;
        ++expectedShadowLightIndex;
    }
    EXPECT_EQ(factory.program->ints["_ShadowLightIndex"], expectedShadowLightIndex);
    EXPECT_EQ(
        factory.program->ints["_DirectLights[0].castShadow"] +
        factory.program->ints["_DirectLights[1].castShadow"],
        1);
    EXPECT_EQ(factory.program->ints["_HasShadowMap"], 1);
}

TEST(SceneRendererShadowTest, NonCastingReceiverContributesBoundsButIsNotDrawn)
{
    auto scene = NewRef<Scene>("shadow");
    auto light = scene->EmplaceChild<DirectionalLight>("light");
    light->Direction() = glm::vec3(0.0f, -1.0f, 0.0f);
    auto casterMaterial = NewRef<Material>("caster", 1);
    casterMaterial->castShadow = true;
    casterMaterial->receiveShadows = false;
    auto receiverMaterial = NewRef<Material>("receiver", 2);
    receiverMaterial->castShadow = false;
    receiverMaterial->receiveShadows = true;
    auto ignoredMaterial = NewRef<Material>("ignored", 3);
    ignoredMaterial->castShadow = false;
    ignoredMaterial->receiveShadows = false;
    auto caster = scene->EmplaceChild<SceneNode>("caster");
    caster->AddComponent<MeshComponent>()->SetMesh(MakeShadowTestMesh(casterMaterial));
    auto receiver = scene->EmplaceChild<SceneNode>("receiver");
    receiver->SetPosition(glm::vec3(100.0f, 0.0f, 0.0f));
    receiver->AddComponent<MeshComponent>()->SetMesh(MakeShadowTestMesh(receiverMaterial));
    auto ignored = scene->EmplaceChild<SceneNode>("ignored");
    ignored->SetPosition(glm::vec3(1000.0f, 0.0f, 0.0f));
    ignored->AddComponent<MeshComponent>()->SetMesh(MakeShadowTestMesh(ignoredMaterial));

    auto rendering = NewRef<ShadowTestRendering>();
    ShadowTestFactory factory;
    RenderSystem renderSystem(rendering, &factory);
    ShadowPass pass;
    ASSERT_TRUE(pass.Init(factory));
    SceneRenderingContext context{ .scene = scene.get() };
    pass.Begin(renderSystem, context);
    pass.Render(renderSystem, context);
    pass.End(renderSystem, context);

    EXPECT_EQ(rendering->draws, 1);
    ASSERT_EQ(context.shadowMaps.size(), 1u);
    const auto& lightSpaceMatrix = context.shadowMaps.front().lightSpaceMatrix;
    const auto receiverClip = lightSpaceMatrix * glm::vec4(100.0f, 0.0f, 0.0f, 1.0f);
    EXPECT_LE(std::abs(receiverClip.x / receiverClip.w), 1.0f);
    EXPECT_LE(std::abs(receiverClip.y / receiverClip.w), 1.0f);
    EXPECT_LE(std::abs(receiverClip.z / receiverClip.w), 1.0f);
    const auto ignoredClip = lightSpaceMatrix * glm::vec4(1000.0f, 0.0f, 0.0f, 1.0f);
    EXPECT_GT(std::abs(ignoredClip.x / ignoredClip.w), 1.0f);

    casterMaterial->castShadow = false;
    casterMaterial->receiveShadows = false;
    SceneRenderingContext noCastersContext{ .scene = scene.get() };
    pass.Begin(renderSystem, noCastersContext);
    pass.Render(renderSystem, noCastersContext);
    pass.End(renderSystem, noCastersContext);
    EXPECT_EQ(rendering->draws, 1);
    EXPECT_TRUE(noCastersContext.shadowMaps.empty());
    LightParamsTestPass lightParamsPass;
    lightParamsPass.SetDirectLightParams(*factory.program, *scene);
    lightParamsPass.SetShadowMapParams(*factory.program, noCastersContext);
    EXPECT_EQ(factory.program->ints["_ShadowLightIndex"], -1);
    EXPECT_EQ(factory.program->ints["_HasShadowMap"], 0);
}