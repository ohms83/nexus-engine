#include "gtest/gtest.h"
#include "nexus/Nexus.h"
#include "helpers/TestRenderingInterface.h"

USING_NAMESPACE_NXS;

TEST(MeshTest, SerializeDeserialize)
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
    MaterialManager mm;
    TextureManager tm(std::make_shared<FakeRendering>());
    // Create placeholder material and texture in managers
    auto createdMat = mm.Create("path/to/mat");
    auto tex = tm.Create("assets/tex.png");
    // Set up material's texture to match createdTexture
    createdMat->AddTexture(tex, TextureType::Diffuse);
    // Now resolve the mesh
    mesh2.Resolve(mm, tm);
    EXPECT_NE(mesh2.GetMaterial(), nullptr);
    EXPECT_FLOAT_EQ(mesh2.GetSphere().center.x, 1.0f);
    EXPECT_FLOAT_EQ(mesh2.GetSphere().radius, 5.0f);
    EXPECT_FLOAT_EQ(mesh2.GetBox().center.x, 0.0f);
}
