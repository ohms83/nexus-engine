#include "gtest/gtest.h"
#include "nexus/Nexus.h"
#include "helpers/TestRenderingInterface.h"

USING_NAMESPACE_NXS;

TEST(MaterialTest, SerializeDeserialize)
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
    EXPECT_EQ(m2.blendMode, m.blendMode);
    EXPECT_EQ(m2.depthFunction, m.depthFunction);
    EXPECT_EQ(m2.TextureCount(), 1u);
    EXPECT_EQ(m2.GetTextureType(0), TextureType::Diffuse);
    EXPECT_EQ(m2.GetTextureUniform(0), "_DiffuseMap");
    EXPECT_EQ(m2.GetTexturePath(0), "assets/tex.png");

    // Use test helper FakeRendering to create a TextureManager
    FakeRendering fakeR;

    auto renderingInterface = std::make_shared<FakeRendering>();
    TextureManager tm(renderingInterface);
    // Insert a dummy texture into the manager's cache by using Create (no loader needed)
    tex = tm.Create<Texture>("assets/tex.png");
    EXPECT_NE(tex, nullptr);
    m2.Resolve(tm, nullptr);
    // Since we did not add material to material manager, resolve will not populate m2 textures via material manager here.
}
