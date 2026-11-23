#include "gtest/gtest.h"
#include "nexus/Nexus.h"
#include "helpers/TestRenderingInterface.h"

USING_NAMESPACE_NXS;

namespace
{
    class MaterialTestProgram final : public GpuProgram
    {
    public:
        void Bind() override {}
        bool IsBinding() const override { return false; }
        bool SetUniformInt(const std::string& name, int32_t value) override
        {
            if (name == "_Material.receiveShadows") receiveShadows = static_cast<float>(value);
            return true;
        }
        bool SetUniformVector(const std::string&, const glm::vec3&) override { return true; }
        bool SetUniformFloat(const std::string& name, float value) override
        {
            if (name == "_Material.receiveShadows") receiveShadows = value;
            return true;
        }

        float receiveShadows = -1.0f;

    private:
        uint32_t Alloc() override { return 1; }
    };

    class MaterialTestRendering final : public FakeRendering
    {
    public:
        Ref<GpuProgram> CreateGpuProgram() const override { return program; }
        Ref<MaterialTestProgram> program = std::make_shared<MaterialTestProgram>();
    };

    class VariantResource final : public Resource
    {
    public:
        VariantResource(std::string path, const uint32_t id, const uint32_t variant)
            : Resource(std::move(path), id)
            , variant(variant)
        {
        }

        uint32_t variant;
    };

    class VariantResourceLoader final : public IResourceLoader
    {
    public:
        Ref<Resource> Load(const std::string& path, const uint32_t id) override
        {
            return Load(path, id, 0);
        }

        Ref<Resource> Load(const std::string& path, const uint32_t id, const uint32_t variant) override
        {
            return std::make_shared<VariantResource>(path, id, variant);
        }

        Ref<LoadResult> LoadAsync(
            const std::string&,
            uint32_t,
            TaskScheduler&,
            Callback) override
        {
            return nullptr;
        }
    };
}

class MaterialTest : public ::testing::Test {
protected:
    ResourceManager resourceManager;
    void SetUp() override
    {
        resourceManager.RegisterLoader(typeid(Texture), std::make_unique<TextureLoader>());
    }

    void TearDown() override
    {
        resourceManager.EmptyCache();
    }
};

TEST_F(MaterialTest, SerializeDeserialize)
{
    Material m("mat", 1);
    m.ambient = Color3F{0.1f, 0.2f, 0.3f};
    m.diffuse = Color3F{0.4f, 0.5f, 0.6f};
    m.specular = Color3F{0.7f, 0.8f, 0.9f};
    m.emissive = Color3F{0.0f, 0.1f, 0.2f};
    m.shininess = 12.5f;
    m.wireframe = true;
    m.cull = true;
    m.depthTest = true;
    m.depthWrite = true;
    m.castShadow = false;
    m.receiveShadows = false;
    m.blendMode = BlendMode::Alpha;
    m.depthFunction = DepthFunction::Equal;

    Ref<Texture> tex = std::make_shared<Texture>("assets/tex.png", 10);
    m.AddTexture(tex, TextureType::Diffuse);

    Ref<Shader> s = std::make_shared<Shader>("shaders/myshader", 11);
    // set shader to serialize a path
    m.SetShader(s);

    VariantData serialized = m.Serialize();

    Material m2("mat2", 2);
    m2.Deserialize(serialized);

    EXPECT_FLOAT_EQ(m2.ambient.r, m.ambient.r);
    EXPECT_FLOAT_EQ(m2.diffuse.r, m.diffuse.r);
    EXPECT_FLOAT_EQ(m2.specular.r, m.specular.r);
    EXPECT_FLOAT_EQ(m2.emissive.r, m.emissive.r);
    EXPECT_FLOAT_EQ(m2.shininess, m.shininess);
    EXPECT_EQ(m2.wireframe, m.wireframe);
    EXPECT_EQ(m2.cull, m.cull);
    EXPECT_EQ(m2.depthTest, m.depthTest);
    EXPECT_EQ(m2.depthWrite, m.depthWrite);
    EXPECT_EQ(m2.castShadow, m.castShadow);
    EXPECT_EQ(m2.receiveShadows, m.receiveShadows);
    EXPECT_EQ(m2.blendMode, m.blendMode);
    EXPECT_EQ(m2.depthFunction, m.depthFunction);
    EXPECT_EQ(m2.TextureCount(), 1u);
    EXPECT_EQ(m2.GetTextureType(0), TextureType::Diffuse);
    EXPECT_EQ(m2.GetTextureUniform(0), "_DiffuseMap");
    EXPECT_EQ(m2.GetTexturePath(0), "assets/tex.png");

    // Use test helper FakeRendering to create a TextureManager
    // Insert a dummy texture into the manager's cache by using Create (no loader needed)
    tex = resourceManager.Create<Texture>("assets/tex.png");
    EXPECT_NE(tex, nullptr);
    m2.Resolve(resourceManager);
    // Since we did not add material to material manager, resolve will not populate m2 textures via material manager here.
}

TEST_F(MaterialTest, LegacyMaterialDefaultsToCastingAndReceivingShadows)
{
    Material source("source", 1);
    auto serialized = source.Serialize();
    serialized.GetMap().erase("castShadow");
    serialized.GetMap().erase("receiveShadows");
    Material restored("restored", 2);
    ASSERT_TRUE(restored.Deserialize(serialized));
    EXPECT_TRUE(restored.castShadow);
    EXPECT_TRUE(restored.receiveShadows);
}

TEST_F(MaterialTest, UseUploadsReceiveShadowsFlag)
{
    MaterialTestRendering rendering;
    auto shader = std::make_shared<Shader>("shader", 1);
    ASSERT_TRUE(shader->CompileFromSource(rendering, "void main() {}", "void main() {}", ""));
    Material material("material", 2);
    material.SetShader(shader);

    ASSERT_NE(material.Use(), nullptr);
    EXPECT_FLOAT_EQ(rendering.program->receiveShadows, 1.0f);
    material.receiveShadows = false;
    ASSERT_NE(material.Use(), nullptr);
    EXPECT_FLOAT_EQ(rendering.program->receiveShadows, 0.0f);
}

TEST_F(MaterialTest, MetallicRoughnessPropertiesSerialize)
{
    Material material("pbr", 1);
    material.shadingModel = MaterialShadingModel::MetallicRoughness;
    material.metalness = 0.65f;
    material.roughness = 0.3f;

    VariantData serialized = material.Serialize();
    Material restored("restored", 2);
    ASSERT_TRUE(restored.Deserialize(serialized));

    EXPECT_EQ(restored.shadingModel, MaterialShadingModel::MetallicRoughness);
    EXPECT_FLOAT_EQ(restored.metalness, 0.65f);
    EXPECT_FLOAT_EQ(restored.roughness, 0.3f);
}

TEST_F(MaterialTest, MetallicOrRoughnessTextureEnablesPbr)
{
    Material material("pbr", 1);
    auto texture = std::make_shared<Texture>("assets/metalness.png", 10);

    ASSERT_GE(material.AddTexture(texture, TextureType::Metalness), 0);

    EXPECT_EQ(material.shadingModel, MaterialShadingModel::MetallicRoughness);
}

TEST_F(MaterialTest, ResourceManagerSeparatesLoaderVariants)
{
    resourceManager.RegisterLoader(typeid(VariantResource), std::make_unique<VariantResourceLoader>());

    const auto linear = resourceManager.Get<VariantResource>("shared-resource");
    const auto srgb = resourceManager.Get<VariantResource>("shared-resource", 1);

    ASSERT_NE(linear, nullptr);
    ASSERT_NE(srgb, nullptr);
    EXPECT_NE(linear, srgb);
    EXPECT_EQ(linear->variant, 0u);
    EXPECT_EQ(srgb->variant, 1u);
    EXPECT_EQ(resourceManager.Get<VariantResource>("shared-resource", 1), srgb);
    EXPECT_TRUE(resourceManager.IsExist(typeid(VariantResource), "shared-resource", 1));
    EXPECT_TRUE(resourceManager.Unload(typeid(VariantResource), "shared-resource", 1));
    EXPECT_FALSE(resourceManager.IsExist(typeid(VariantResource), "shared-resource", 1));
    EXPECT_TRUE(resourceManager.IsExist(typeid(VariantResource), "shared-resource"));
}
