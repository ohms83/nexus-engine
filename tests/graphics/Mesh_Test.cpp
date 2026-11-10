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

TEST_F(MeshTest, PrimitiveCreatePlane)
{
    auto fakeRendering = std::make_shared<FakeRendering>();
    auto material = resourceManager.Create<Material>("path/to/mat");
    auto planeMesh = PrimitiveMesh::CreatePlane("plane", 2.0f, 3.0f, *fakeRendering, material);
    ASSERT_NE(planeMesh, nullptr);
    EXPECT_EQ(planeMesh->GetName(), "plane");
    ASSERT_NE(planeMesh->GetVertexBuffer(), nullptr);
    ASSERT_NE(planeMesh->GetIndexBuffer(), nullptr);
    EXPECT_EQ(planeMesh->GetMaterial(), material);
    EXPECT_EQ(planeMesh->GetVertexBuffer()->VertexCount(), 4u);
    EXPECT_EQ(planeMesh->GetIndexBuffer()->NumIndex(), 6u);
    EXPECT_EQ(planeMesh->GetIndexBuffer()->GetBufferSize(), 6u * sizeof(uint32_t));

    for (uint32_t i = 0; i < planeMesh->GetIndexBuffer()->NumIndex(); ++i)
    {
        EXPECT_LT(planeMesh->GetIndexBuffer()->GetData()[i], planeMesh->GetVertexBuffer()->VertexCount());
    }

    planeMesh.reset();
}

TEST_F(MeshTest, PrimitiveCreateBox)
{
    FakeRendering fakeRendering;
    auto boxMesh = PrimitiveMesh::CreateBox("box", {2.0f, 3.0f, 4.0f}, fakeRendering);

    ASSERT_NE(boxMesh, nullptr);
    ASSERT_NE(boxMesh->GetVertexBuffer(), nullptr);
    ASSERT_NE(boxMesh->GetIndexBuffer(), nullptr);
    EXPECT_EQ(boxMesh->GetName(), "box");
    EXPECT_EQ(boxMesh->GetVertexBuffer()->VertexCount(), 24u);
    EXPECT_EQ(boxMesh->GetIndexBuffer()->NumIndex(), 36u);
    EXPECT_EQ(boxMesh->GetIndexBuffer()->GetBufferSize(), 36u * sizeof(uint32_t));

    for (uint32_t i = 0; i < boxMesh->GetIndexBuffer()->NumIndex(); ++i)
    {
        EXPECT_LT(boxMesh->GetIndexBuffer()->GetData()[i], boxMesh->GetVertexBuffer()->VertexCount());
    }

    boxMesh.reset();
}

TEST_F(MeshTest, PrimitiveCreateSphere)
{
    constexpr uint32_t sectorCount = 8;
    constexpr uint32_t stackCount = 4;
    FakeRendering fakeRendering;
    auto sphereMesh = PrimitiveMesh::CreateSphere(
        "sphere",
        2.0f,
        sectorCount,
        stackCount,
        fakeRendering);

    ASSERT_NE(sphereMesh, nullptr);
    ASSERT_NE(sphereMesh->GetVertexBuffer(), nullptr);
    ASSERT_NE(sphereMesh->GetIndexBuffer(), nullptr);
    EXPECT_EQ(sphereMesh->GetName(), "sphere");
    EXPECT_EQ(sphereMesh->GetVertexBuffer()->VertexCount(), (sectorCount + 1) * (stackCount + 1));
    EXPECT_EQ(sphereMesh->GetIndexBuffer()->NumIndex(), sectorCount * (stackCount - 1) * 6);
    EXPECT_EQ(
        sphereMesh->GetIndexBuffer()->GetBufferSize(),
        sectorCount * (stackCount - 1) * 6 * sizeof(uint32_t));

    for (uint32_t i = 0; i < sphereMesh->GetIndexBuffer()->NumIndex(); ++i)
    {
        EXPECT_LT(sphereMesh->GetIndexBuffer()->GetData()[i], sphereMesh->GetVertexBuffer()->VertexCount());
    }

    sphereMesh.reset();
}
