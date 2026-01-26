#include "gtest/gtest.h"
#include "nexus/Nexus.h"
#include "helpers/TestRenderingInterface.h"

USING_NAMESPACE_NXS;

class MeshTest : public ::testing::Test {
protected:
    ResourceManager resourceManager;
    void SetUp() override
    {
        resourceManager.RegisterLoader(typeid(Material), std::make_unique<MaterialLoader>(
            std::make_shared<FakeRendering>(),
            resourceManager));
        resourceManager.RegisterLoader(typeid(Texture), std::make_unique<TextureLoader>(std::make_shared<FakeRendering>()));
    }

    void TearDown() override
    {
        resourceManager.EmptyCache();
    }
};


TEST_F(MeshTest, SerializeDeserialize)
{
    Mesh mesh("testmesh");
    mesh.SetSphere(Sphere(glm::vec3{1.0f, 2.0f, 3.0f}, 5.0f));
    mesh.SetBox(Box(glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{1.0f, 1.0f, 1.0f}));
    auto mat = std::make_shared<Material>("path/to/mat", 123);
    mesh.SetMaterial(mat);
    VariantData serialized = mesh.Serialize();
    // Serialized output should contain the material path used
    EXPECT_EQ(serialized["material"].GetString(), std::string("path/to/mat"));

    Mesh mesh2;
    mesh2.Deserialize(serialized);
    EXPECT_EQ(mesh2.GetName(), mesh.GetName());
    // Create a simple resource manager environment for resolution
    auto fakeRendering = std::make_shared<FakeRendering>();
    // Create placeholder material and texture in managers
    auto createdMat = resourceManager.Create<Material>("path/to/mat");
    auto tex = resourceManager.Create<Texture>("assets/tex.png");
    // Set up material's texture to match createdTexture
    createdMat->AddTexture(tex, TextureType::Diffuse);
    // Now resolve the mesh
    mesh2.Resolve(resourceManager);
    EXPECT_NE(mesh2.GetMaterial(), nullptr);
    EXPECT_FLOAT_EQ(mesh2.GetSphere().center.x, 1.0f);
    EXPECT_FLOAT_EQ(mesh2.GetSphere().radius, 5.0f);
    EXPECT_FLOAT_EQ(mesh2.GetBox().center.x, 0.0f);
}
