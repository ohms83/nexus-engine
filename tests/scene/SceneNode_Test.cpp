#include "gtest/gtest.h"
#include "nexus/Nexus.h"
#include <random>

USING_NAMESPACE_NXS;

class SceneNodeTest : public ::testing::Test {
protected:
    Ptr<Scene> scene;
    Ref<TaskScheduler> scheduler;

    void SetUp() override {
        scheduler = std::make_shared<TaskScheduler>(std::make_shared<StandardTimeSource>());

        scene = std::make_unique<Scene>("Test Scene");
        scene->SetTaskScheduler(scheduler);
    }

    void TearDown() override {
        scheduler.reset();
        scene.reset();
    }
};

TEST_F(SceneNodeTest, AddNode)
{
    // The specified node shouldn't have already existed before adding.
    const std::string name = "Test Node";
    auto emptyNode = scene->FindNodeWithName(name);
    EXPECT_EQ(emptyNode, nullptr);

    auto node = scene->EmplaceChild<SceneNode3D>(name);
    EXPECT_NE(node, nullptr);
    EXPECT_EQ(node->GetName(), name);
}

TEST_F(SceneNodeTest, RemoveNode)
{
    const std::string name = "Test Node";

    auto node = scene->EmplaceChild<SceneNode>(name);
    EXPECT_NE(node, nullptr);
    EXPECT_EQ(node->GetName(), name);
    EXPECT_NE(scene->FindNodeWithName(name), nullptr);

    scene->RemoveChild(node);
    // The node shouldn't have been removed yet.
    EXPECT_NE(scene->FindNodeWithName(name), nullptr);

    scheduler->PostUpdate();
    EXPECT_EQ(scene->FindNodeWithName(name), nullptr);
}

// Testing for a bug where removing a scene node will have side effects on the others
// due to dangling references.
TEST_F(SceneNodeTest, DanglingReferences)
{
    const auto name1 = "Node 1";
    const auto name2 = "Node 2";

    auto registry = std::make_shared<entt::registry>();
    auto node1 = std::make_shared<SceneNode>(registry, name1);
    auto node2 = std::make_shared<SceneNode>(registry, name2);
    
    EXPECT_NE(node1, node2);
    EXPECT_NE(node1->GetName(), node2->GetName());
    EXPECT_EQ(node1->GetName(), name1);
    EXPECT_EQ(node2->GetName(), name2);

    node1.reset();
    EXPECT_EQ(node1, nullptr);
    EXPECT_EQ(node2->GetName(), name2);
}