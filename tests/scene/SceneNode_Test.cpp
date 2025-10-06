#include "gtest/gtest.h"
#include "nexus/Nexus.h"
#include <random>

USING_NAMESPACE_NXS;

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
    auto emptyNode = scene->FindNode("Test");
    EXPECT_EQ(emptyNode, nullptr);

    nxs::Random rand;
    auto node = scene->CreateNode<SceneNode3D>("Test");
    const auto pos = rand.RangeVec<float>(0, 1);
    node->Position().value = pos;

    const auto anotherNode = PTR_CAST<SceneNode3D>(scene->FindNode("Test"));
    EXPECT_EQ(node, anotherNode);
    EXPECT_EQ(node->GetName(),  anotherNode->GetName());
    EXPECT_EQ(node->Position().value, anotherNode->Position().value);
}

TEST_F(SceneNodeTest, RemoveNode)
{
    const auto name1 = "Node 1";
    const auto name2 = "Node 2";
    auto node1 = scene->CreateNode<SceneNode>(name1);
    auto node2 = scene->CreateNode<SceneNode>(name2);
    
    EXPECT_NE(node1, node2);
    EXPECT_NE(node1->GetName(), node2->GetName());

    scene->RemoveNodeByName(name2);
    node2 = scene->FindNode(name2);
    EXPECT_EQ(node2, nullptr);
    EXPECT_EQ(node1->GetName(), name1);
}