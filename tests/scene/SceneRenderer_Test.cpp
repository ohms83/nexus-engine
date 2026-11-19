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
        bool Init() override { return false; }
    };

    class CountingRenderer final : public SceneRenderer
    {
    public:
        explicit CountingRenderer(bool fails)
        {
            if (fails) RegisterRenderPass(RenderPassQueue::Opaque, std::make_shared<FailedInitPass>());
        }

        void Render(RenderSystem&, const Scene&) override { ++renders; }
        int renders = 0;
    };

    class ShadowTestProgram final : public GpuProgram
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
        bool SetUniformMatrix(const std::string&, const glm::mat4&, bool) override { return true; }
        bool SetUniformTexture2D(const std::string&, Ref<const TextureProxy>, int32_t) override { return true; }

        std::unordered_map<std::string, int32_t> ints;
    };

    class ShadowTestRendering final : public FakeRendering
    {
    public:
        void ClearDepth(float) override {}
        void DrawIndexed(const Ref<IndexBuffer>) override { ++draws; }
        int draws = 0;
    };

    class ShadowTestRenderer final : public SceneRenderer
    {
    public:
        explicit ShadowTestRenderer(const Ref<ShadowTestProgram>& program)
        {
            m_shadowProgram = program;
            m_shadowMap = std::make_shared<FakeRenderTarget>();
        }
        using SceneRenderer::RenderDirectionalShadowMap;
        using SceneRenderer::SetDirectLightParams;
        using SceneRenderer::SetShadowMapParams;
        const glm::mat4& LightSpaceMatrix() const { return m_lightSpaceMatrix; }
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
        auto mesh = std::make_shared<Mesh>("test");
        mesh->SetMaterial(material);
        mesh->SetSphere(Sphere(glm::vec3(0.0f), 1.0f));
        mesh->SetVertexBuffer(std::make_shared<FakdeVertexBuffer>());
        mesh->SetIndexBuffer(std::make_shared<FakeIndexBuffer>());
        return mesh;
    }
}

TEST(SceneRendererTest, ExecutesSelectedPassesPerViewWithFreshCommandsAndClearMasks)
{
    auto scene = std::make_shared<Scene>("scene");
    auto first = scene->EmplaceChild<CountingSceneView>("first");
    auto second = scene->EmplaceChild<CountingSceneView>("second");
    auto inactive = scene->EmplaceChild<CountingSceneView>("inactive");
    inactive->Activate(false);
    auto otherScene = std::make_shared<Scene>("other");
    auto foreign = otherScene->EmplaceChild<CountingSceneView>("foreign");
    auto material = std::make_shared<Material>("opaque", 1);
    auto mesh = scene->EmplaceChild<SceneNode>("mesh");
    mesh->AddComponent<MeshComponent>()->SetMesh(MakeShadowTestMesh(material));
    first->visibleNodes.push_back(mesh.get());
    second->visibleNodes.push_back(mesh.get());

    auto depth = std::make_shared<ViewTestPass>();
    auto opaque = std::make_shared<ViewTestPass>();
    first->profile.depthFill = true;
    second->profile.depthFill = true;
    second->profile.culling = CullingFlags::Opaque;
    depth->pipelineState.colorMask = glm::bvec4(false);
    depth->pipelineState.depthWrite = false;
    opaque->pipelineState.colorMask = glm::bvec4(true);
    depth->pipelineState.globalShader = std::make_shared<ViewTestProgram>();
    opaque->pipelineState.globalShader = std::make_shared<ViewTestProgram>();
    int depthBegins = 0, depthEnds = 0, opaqueBegins = 0, opaqueEnds = 0;
    depth->onBegin = [&](RenderSystem&) { ++depthBegins; };
    depth->onEnd = [&](RenderSystem&) { ++depthEnds; };
    opaque->onBegin = [&](RenderSystem&) { ++opaqueBegins; };
    opaque->onEnd = [&](RenderSystem&) { ++opaqueEnds; };

    SceneRenderer renderer;
    renderer.RegisterRenderPass(RenderPassQueue::DepthFill, depth);
    renderer.RegisterRenderPass(RenderPassQueue::Opaque, opaque);
    auto rendering = std::make_shared<ViewTestRendering>();
    RenderSystem renderSystem(rendering);
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
    auto scene = std::make_shared<Scene>("scene");
    auto view = scene->EmplaceChild<CountingSceneView>("empty");
    auto pass = std::make_shared<ViewTestPass>();
    pass->appendedMesh = MakeShadowTestMesh(std::make_shared<Material>("overlay", 1));
    pass->pipelineState.globalShader = std::make_shared<ViewTestProgram>();
    int begins = 0, ends = 0;
    pass->onBegin = [&](RenderSystem&) { ++begins; };
    pass->onEnd = [&](RenderSystem&) { ++ends; };
    SceneRenderer renderer;
    renderer.RegisterRenderPass(RenderPassQueue::Opaque, pass);
    auto rendering = std::make_shared<ViewTestRendering>();
    RenderSystem renderSystem(rendering);
    renderer.Render(renderSystem, *scene);
    EXPECT_EQ(begins, 1);
    EXPECT_EQ(ends, 1);
    EXPECT_EQ(pass->appendCalls, 1);
    EXPECT_EQ(rendering->drawMasks.size(), 1u);
}

TEST(SceneRendererTest, DisabledPassDoesNotRun)
{
    auto scene = std::make_shared<Scene>("scene");
    scene->EmplaceChild<CountingSceneView>("view");
    auto pass = std::make_shared<ViewTestPass>();
    pass->enabled = false;
    SceneRenderer renderer;
    renderer.RegisterRenderPass(RenderPassQueue::Opaque, pass);
    auto rendering = std::make_shared<ViewTestRendering>();
    RenderSystem renderSystem(rendering);

    renderer.Render(renderSystem, *scene);

    EXPECT_EQ(pass->appendCalls, 0);
    EXPECT_TRUE(rendering->drawMasks.empty());
}

TEST(SceneRendererTest, RendersVisibleMeshThroughSceneCamera)
{
    auto scene = std::make_shared<Scene>("scene");
    auto camera = scene->EmplaceChild<Camera>("camera");
    camera->SetPosition({0.0f, 0.0f, 5.0f});
    camera->LookAt(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    camera->SetProjection(60.0f, 800.0f, 600.0f, 0.1f, 100.0f);
    auto mesh = scene->EmplaceChild<SceneNode>("mesh");
    mesh->AddComponent<MeshComponent>()->SetMesh(
        MakeShadowTestMesh(std::make_shared<Material>("opaque", 1)));

    auto pass = std::make_shared<ViewTestPass>();
    pass->pipelineState.globalShader = std::make_shared<ViewTestProgram>();
    SceneRenderer renderer;
    renderer.RegisterRenderPass(RenderPassQueue::Opaque, pass);
    auto rendering = std::make_shared<ViewTestRendering>();
    RenderSystem renderSystem(rendering);
    renderer.Render(renderSystem, *scene);

    EXPECT_EQ(rendering->clears, 1);
    EXPECT_EQ(rendering->drawMasks.size(), 1u);
}

TEST(SceneRendererTest, FailedRendererInitializationKeepsPreviousRenderer)
{
    auto scene = std::make_shared<Scene>("scene");
    auto rendering = std::make_shared<FakeRendering>();
    RenderSystem renderSystem(rendering);
    auto previous = std::make_shared<CountingRenderer>(false);
    auto failing = std::make_shared<CountingRenderer>(true);

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
    auto scene = std::make_shared<Scene>("scene");
    auto parent = scene->EmplaceChild<SceneNode>("parent");
    parent->EmplaceChild<Camera>("camera");
    parent->EmplaceChild<DirectionalLight>("light");
    auto pass = std::make_shared<ViewTestPass>();
    auto rendering = std::make_shared<ViewTestRendering>();
    RenderSystem renderSystem(rendering);
    SceneRenderer viewRenderer;
    viewRenderer.RegisterRenderPass(RenderPassQueue::Opaque, pass);
    auto program = std::make_shared<ShadowTestProgram>();
    ShadowTestRenderer renderer(program);

    viewRenderer.Render(renderSystem, *scene);
    EXPECT_EQ(pass->appendCalls, 1);
    renderer.SetDirectLightParams(program, *scene);
    EXPECT_EQ(program->ints["_NumDirectLight"], 1);

    parent->Activate(false);
    viewRenderer.Render(renderSystem, *scene);
    EXPECT_EQ(pass->appendCalls, 1);
    renderer.SetDirectLightParams(program, *scene);
    EXPECT_EQ(program->ints["_NumDirectLight"], 0);
}

TEST(SceneRendererShadowTest, SelectsEnabledLightAndUpdatesShaderIndex)
{
    auto scene = std::make_shared<Scene>("shadow");
    auto lightA = scene->EmplaceChild<DirectionalLight>("A");
    auto lightB = scene->EmplaceChild<DirectionalLight>("B");
    lightA->Direction() = lightB->Direction() = glm::vec3(0.0f, -1.0f, 0.0f);
    auto view = scene->GetRegistry()->view<SceneNodeComponent, DirectLightComponent>();
    auto it = view.begin();
    const auto firstId = view.get<SceneNodeComponent>(*it).id;
    auto* firstNode = SceneNode::FindNode(firstId);
    auto* first = firstNode->GetComponent<DirectLightComponent>();
    first->properties.canCastShadows = false;
    auto material = std::make_shared<Material>("shadow", 1);
    scene->EmplaceChild<SceneNode>("caster")->AddComponent<MeshComponent>()->SetMesh(MakeShadowTestMesh(material));

    auto program = std::make_shared<ShadowTestProgram>();
    auto rendering = std::make_shared<ShadowTestRendering>();
    RenderSystem renderSystem(rendering);
    ShadowTestRenderer renderer(program);
    renderer.RenderDirectionalShadowMap(renderSystem, *scene);
    renderer.SetDirectLightParams(program, *scene);
    renderer.SetShadowMapParams(program);
    EXPECT_EQ(rendering->draws, 1);
    EXPECT_EQ(program->ints["_NumDirectLight"], 2);
    EXPECT_EQ(program->ints["_ShadowLightIndex"], 1);
    EXPECT_EQ(program->ints["_HasShadowMap"], 1);

    first->properties.canCastShadows = true;
    renderer.RenderDirectionalShadowMap(renderSystem, *scene);
    renderer.SetDirectLightParams(program, *scene);
    EXPECT_EQ(rendering->draws, 2);
    EXPECT_EQ(program->ints["_ShadowLightIndex"], 0);

    firstNode->Activate(false);
    renderer.RenderDirectionalShadowMap(renderSystem, *scene);
    renderer.SetDirectLightParams(program, *scene);
    EXPECT_EQ(rendering->draws, 3);
    EXPECT_EQ(program->ints["_NumDirectLight"], 1);
    EXPECT_EQ(program->ints["_ShadowLightIndex"], 0);

    lightA->Properties().canCastShadows = false;
    lightB->Properties().canCastShadows = false;
    renderer.RenderDirectionalShadowMap(renderSystem, *scene);
    renderer.SetDirectLightParams(program, *scene);
    renderer.SetShadowMapParams(program);
    EXPECT_EQ(rendering->draws, 3);
    EXPECT_EQ(program->ints["_ShadowLightIndex"], -1);
    EXPECT_EQ(program->ints["_HasShadowMap"], 0);
}

TEST(SceneRendererShadowTest, NonCastingReceiverContributesBoundsButIsNotDrawn)
{
    auto scene = std::make_shared<Scene>("shadow");
    auto light = scene->EmplaceChild<DirectionalLight>("light");
    light->Direction() = glm::vec3(0.0f, -1.0f, 0.0f);
    auto casterMaterial = std::make_shared<Material>("caster", 1);
    casterMaterial->receiveShadows = false;
    auto receiverMaterial = std::make_shared<Material>("receiver", 2);
    receiverMaterial->castShadow = false;
    auto ignoredMaterial = std::make_shared<Material>("ignored", 3);
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

    auto program = std::make_shared<ShadowTestProgram>();
    auto rendering = std::make_shared<ShadowTestRendering>();
    RenderSystem renderSystem(rendering);
    ShadowTestRenderer renderer(program);
    renderer.RenderDirectionalShadowMap(renderSystem, *scene);
    EXPECT_EQ(rendering->draws, 1);
    const auto clip = renderer.LightSpaceMatrix() * glm::vec4(100.0f, 0.0f, 0.0f, 1.0f);
    EXPECT_LE(std::abs(clip.x / clip.w), 1.0f);
    EXPECT_LE(std::abs(clip.y / clip.w), 1.0f);
    EXPECT_LE(std::abs(clip.z / clip.w), 1.0f);
    const auto ignoredClip = renderer.LightSpaceMatrix() * glm::vec4(1000.0f, 0.0f, 0.0f, 1.0f);
    EXPECT_GT(std::abs(ignoredClip.x / ignoredClip.w), 1.0f);

    casterMaterial->castShadow = false;
    renderer.RenderDirectionalShadowMap(renderSystem, *scene);
    renderer.SetShadowMapParams(program);
    EXPECT_EQ(rendering->draws, 1);
    EXPECT_EQ(program->ints["_HasShadowMap"], 0);
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