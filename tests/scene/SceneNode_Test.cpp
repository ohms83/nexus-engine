#include "gtest/gtest.h"
#include "nexus/Nexus.h"
#include <random>

USING_NAMESPACE_NXS;

// Define a test fixture for HighResTimeSource
class SceneNodeTest : public ::testing::Test {
protected:
    Ptr<Scene> scene;
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<> dis;

    void SetUp() override {
        scene = std::make_unique<Scene>();
        
        gen = std::mt19937(rd());
        dis = std::uniform_real_distribution<>(0, 1);
    }

    void TearDown() override {
        scene.reset();
    }
};

TEST_F(SceneNodeTest, AddNode)
{
    auto node = scene->CreateNode<SceneNode3D>("Test");
    const auto pos = glm::vec3(dis(gen), dis(gen), dis(gen));
    node->Position().value = pos;

    const auto anotherNode = PTR_CAST<SceneNode3D>(scene->GetNode("Test"));
    EXPECT_TRUE(anotherNode->GetName() == "Test");
    EXPECT_TRUE(anotherNode->Position().value == pos);
}