#include "gtest/gtest.h"
#include "nexus/Nexus.h"
#include <random>

USING_NAMESPACE_NXS;

class SceneNodeTest : public ::testing::Test {
protected:
    Ref<Scene> scene;

    void SetUp() override {
        scene = std::make_shared<Scene>("Test Scene");
    }

    void TearDown() override {
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
    EXPECT_EQ(scene->FindNodeWithName(name), nullptr);
}

TEST_F(SceneNodeTest, RemoveChild_RemovesDescendantsByDefault)
{
    auto parent = scene->EmplaceChild<SceneNode>("Parent");
    auto child = parent->EmplaceChild<SceneNode>("Child");
    auto grandchild = child->EmplaceChild<SceneNode>("Grandchild");

    EXPECT_NE(parent->FindNodeWithName("Child"), nullptr);
    EXPECT_NE(parent->FindNodeWithName("Grandchild"), nullptr);

    parent->RemoveChild(child);
    // The default behavior removes descendants as well
    EXPECT_EQ(parent->FindNodeWithName("Child"), nullptr);
    EXPECT_EQ(parent->FindNodeWithName("Grandchild"), nullptr);
}

TEST_F(SceneNodeTest, RemoveChild_ReparentsChildrenWhenRequested)
{
    auto parent = scene->EmplaceChild<SceneNode>("Parent");
    auto child = parent->EmplaceChild<SceneNode>("Child");
    auto grandchild = child->EmplaceChild<SceneNode>("Grandchild");

    parent->RemoveChild(child, false);
    // Child should be removed but the direct child (grandchild) should be reparented to parent
    EXPECT_EQ(parent->FindNodeWithName("Child"), nullptr);
    auto foundGrandchild = parent->FindNodeWithName("Grandchild");
    EXPECT_NE(foundGrandchild, nullptr);
    EXPECT_EQ(foundGrandchild->GetParent(), parent);
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

TEST_F(SceneNodeTest, SetName)
{
    auto node = scene->EmplaceChild<SceneNode>("Original Name");
    EXPECT_EQ(node->GetName(), "Original Name");

    node->SetName("New Name");
    EXPECT_EQ(node->GetName(), "New Name");
}

TEST_F(SceneNodeTest, ActivateDeactivate)
{
    auto node = scene->EmplaceChild<SceneNode>("Test Node");
    EXPECT_TRUE(node->IsActive());

    node->Activate(false);
    EXPECT_FALSE(node->IsActive());

    node->Activate(true);
    EXPECT_TRUE(node->IsActive());
}

TEST_F(SceneNodeTest, ParentChildRelationship)
{
    auto parent = scene->EmplaceChild<SceneNode>("Parent");
    auto child = parent->EmplaceChild<SceneNode>("Child");

    EXPECT_EQ(child->GetParent(), parent);
    EXPECT_EQ(parent->GetParent(), PTR_CAST<SceneNode>(scene));

    child->RemoveFromParent();

    EXPECT_EQ(child->GetParent(), nullptr);
    EXPECT_EQ(parent->GetNumChildren(), 0);
}

TEST_F(SceneNodeTest, GetAllChildren)
{
    auto parent = scene->EmplaceChild<SceneNode>("Parent");
    auto child1 = parent->EmplaceChild<SceneNode>("Child1");
    auto child2 = parent->EmplaceChild<SceneNode>("Child2");

    SceneNode::ChildList children;
    parent->GetAllChildren(children);

    EXPECT_EQ(children.size(), 2);
    EXPECT_TRUE(std::find(children.begin(), children.end(), child1) != children.end());
    EXPECT_TRUE(std::find(children.begin(), children.end(), child2) != children.end());
}

TEST_F(SceneNodeTest, GetAllDescendants)
{
    auto parent = scene->EmplaceChild<SceneNode>("Parent");
    auto child1 = parent->EmplaceChild<SceneNode>("Child1");
    auto grandchild = child1->EmplaceChild<SceneNode>("Grandchild");
    auto child2 = parent->EmplaceChild<SceneNode>("Child2");

    SceneNode::ChildList descendants;
    parent->GetAllDescendants(descendants, true); // parentFirst

    EXPECT_EQ(descendants.size(), 3);
    EXPECT_TRUE(std::find(descendants.begin(), descendants.end(), child2) != descendants.end());
    EXPECT_TRUE(std::find(descendants.begin(), descendants.end(), child1) != descendants.end());
    EXPECT_TRUE(std::find(descendants.begin(), descendants.end(), grandchild) != descendants.end());
}

TEST_F(SceneNodeTest, FindNodeById)
{
    auto node = scene->EmplaceChild<SceneNode>("Test Node");
    auto found = scene->FindNode(node->GetId());
    EXPECT_EQ(found, node);
}

TEST_F(SceneNodeTest, RemoveAllChildren)
{
    auto parent = scene->EmplaceChild<SceneNode>("Parent");
    parent->EmplaceChild<SceneNode>("Child1");
    parent->EmplaceChild<SceneNode>("Child2");

    EXPECT_EQ(parent->GetNumChildren(), 2);

    parent->RemoveAllChildren();

    EXPECT_EQ(parent->GetNumChildren(), 0);
}

TEST_F(SceneNodeTest, HasChildAndGetNumChildren)
{
    auto parent = scene->EmplaceChild<SceneNode>("Parent");
    EXPECT_FALSE(parent->HasChild());
    EXPECT_EQ(parent->GetNumChildren(), 0);

    auto child = parent->EmplaceChild<SceneNode>("Child");
    EXPECT_TRUE(parent->HasChild());
    EXPECT_EQ(parent->GetNumChildren(), 1);

    parent->RemoveAllChildren();

    EXPECT_FALSE(parent->HasChild());
    EXPECT_EQ(parent->GetNumChildren(), 0);
}