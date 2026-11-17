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
    NODISCARD RenderTarget* GetRenderTarget() const override { return nullptr; }
    void SetRenderTarget(RenderTarget*) override {}

    NODISCARD Array<SceneNode*> GetVisibleNodes(const Scene&) const override
    {
        ++visibilityCalls;
        return {};
    }

    mutable uint32_t visibilityCalls = 0;

private:
    glm::vec3 m_position{0.0f};
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
            if (fails) RegisterRenderPass(std::make_shared<FailedInitPass>());
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
    RenderPass pass;
    auto program = std::make_shared<ShadowTestProgram>();
    ShadowTestRenderer renderer(program);

    EXPECT_EQ(pass.PopulateSceneView(*scene).size(), 1u);
    renderer.SetDirectLightParams(program, *scene);
    EXPECT_EQ(program->ints["_NumDirectLight"], 1);

    parent->Activate(false);
    EXPECT_TRUE(pass.PopulateSceneView(*scene).empty());
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