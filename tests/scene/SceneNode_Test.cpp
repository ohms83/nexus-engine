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

TEST_F(SceneNodeTest, RemoveChild_ReparentsDescendantsByDefault)
{
    auto parent = scene->EmplaceChild<SceneNode>("Parent");
    auto child = parent->EmplaceChild<SceneNode>("Child");
    auto grandchild = child->EmplaceChild<SceneNode>("Grandchild");

    parent->RemoveChild(child);

    EXPECT_EQ(child->GetParent(), nullptr);
    EXPECT_EQ(child->GetNumChildren(), 0u);
    EXPECT_EQ(parent->FindNodeWithName("Child"), nullptr);
    EXPECT_EQ(parent->FindNodeWithName("Grandchild"), grandchild);
    EXPECT_EQ(grandchild->GetParent(), parent);
}

TEST_F(SceneNodeTest, RemoveChild_KeepsDescendantsWithDetachedNodeWhenNotReparenting)
{
    auto parent = scene->EmplaceChild<SceneNode>("Parent");
    auto child = parent->EmplaceChild<SceneNode>("Child");
    auto grandchild = child->EmplaceChild<SceneNode>("Grandchild");

    parent->RemoveChild(child, false);

    EXPECT_EQ(child->GetParent(), nullptr);
    EXPECT_EQ(parent->GetNumChildren(), 0u);
    EXPECT_EQ(parent->FindNodeWithName("Child"), nullptr);
    EXPECT_EQ(parent->FindNodeWithName("Grandchild"), nullptr);
    EXPECT_EQ(child->GetNumChildren(), 1u);
    EXPECT_EQ(child->FindNodeWithName("Grandchild"), grandchild);
    EXPECT_EQ(grandchild->GetParent(), child);
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

TEST_F(SceneNodeTest, ParentReferenceDoesNotKeepParentAlive)
{
    auto registry = std::make_shared<entt::registry>();
    auto parent = std::make_shared<SceneNode>(registry, "Parent");
    auto child = std::make_shared<SceneNode>(registry, "Child");
    std::weak_ptr<SceneNode> weakParent = parent;
    parent->AddChild(child);

    parent.reset();

    EXPECT_TRUE(weakParent.expired());
    EXPECT_EQ(child->GetParent(), nullptr);
}

TEST_F(SceneNodeTest, LocalAndGlobalTransformsComposeFromRootToLeaf)
{
    auto parent = scene->EmplaceChild<SceneNode3D>("Transform Parent");
    parent->SetPosition({2.0f, 0.0f, 0.0f});
    parent->SetOrientation(glm::quat(glm::radians(glm::vec3(0.0f, 90.0f, 0.0f))));
    parent->SetScale({2.0f, 2.0f, 2.0f});

    auto child = parent->EmplaceChild<SceneNode3D>("Transform Child");
    child->SetPosition({1.0f, 0.0f, 0.0f});
    child->SetScale({0.5f, 1.0f, 1.0f});

    const auto localPosition = child->GetLocalTransform() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    EXPECT_EQ(glm::vec3(localPosition), glm::vec3(1.0f, 0.0f, 0.0f));

    const auto globalPosition = child->GetGlobalPosition();
    EXPECT_NEAR(globalPosition.x, 2.0f, 0.0001f);
    EXPECT_NEAR(globalPosition.y, 0.0f, 0.0001f);
    EXPECT_NEAR(globalPosition.z, -2.0f, 0.0001f);

    const auto globalScale = child->GetGlobalScale();
    EXPECT_NEAR(globalScale.x, 1.0f, 0.0001f);
    EXPECT_NEAR(globalScale.y, 2.0f, 0.0001f);
    EXPECT_NEAR(globalScale.z, 2.0f, 0.0001f);
}

TEST_F(SceneNodeTest, LocalTransformEnforcesAffineForm)
{
    auto node = scene->EmplaceChild<SceneNode>("Affine Node");
    auto transform = glm::mat4(1.0f);
    transform[0][3] = 2.0f;
    transform[1][3] = -3.0f;
    transform[2][3] = 4.0f;
    transform[3][3] = 5.0f;
    node->SetLocalTransform(transform);

    const auto affineTransform = node->GetLocalTransform();
    EXPECT_EQ(affineTransform[0][3], 0.0f);
    EXPECT_EQ(affineTransform[1][3], 0.0f);
    EXPECT_EQ(affineTransform[2][3], 0.0f);
    EXPECT_EQ(affineTransform[3][3], 1.0f);
}

TEST_F(SceneNodeTest, CameraViewMatrixInvertsWorldPositionAndOrientation)
{
    auto camera = scene->EmplaceChild<Camera>("Camera");
    camera->SetPosition({3.0f, -2.0f, 5.0f});
    camera->SetOrientation(glm::quat(glm::radians(glm::vec3(15.0f, 40.0f, -10.0f))));

    const auto worldPosition = camera->GetGlobalPosition();
    const auto worldOrientation = camera->GetGlobalOrientation();
    const auto viewMatrix = camera->GetViewMatrix();
    const auto cameraOrigin = viewMatrix * glm::vec4(worldPosition, 1.0f);
    const auto pointInFront = viewMatrix
        * glm::vec4(worldPosition + worldOrientation * glm::vec3(0.0f, 0.0f, -1.0f), 1.0f);

    EXPECT_NEAR(cameraOrigin.x, 0.0f, 0.0001f);
    EXPECT_NEAR(cameraOrigin.y, 0.0f, 0.0001f);
    EXPECT_NEAR(cameraOrigin.z, 0.0f, 0.0001f);
    EXPECT_NEAR(pointInFront.x, 0.0f, 0.0001f);
    EXPECT_NEAR(pointInFront.y, 0.0f, 0.0001f);
    EXPECT_NEAR(pointInFront.z, -1.0f, 0.0001f);
}

TEST_F(SceneNodeTest, GlobalTransformsIncludeAllAncestors)
{
    const glm::quat parentOrientation(glm::radians(glm::vec3(0.0f, 90.0f, 0.0f)));
    const glm::quat childOrientation(glm::radians(glm::vec3(0.0f, 45.0f, 0.0f)));
    const glm::quat grandchildOrientation(glm::radians(glm::vec3(0.0f, -30.0f, 0.0f)));

    auto parent = scene->EmplaceChild<SceneNode3D>("Parent");
    parent->SetPosition({5.0f, 0.0f, 0.0f});
    parent->SetOrientation(parentOrientation);
    parent->SetScale({2.0f, 2.0f, 2.0f});

    auto child = parent->EmplaceChild<SceneNode3D>("Child");
    child->SetPosition({1.0f, 0.0f, 0.0f});
    child->SetOrientation(childOrientation);
    child->SetScale({3.0f, 3.0f, 3.0f});

    auto grandchild = child->EmplaceChild<SceneNode3D>("Grandchild");
    grandchild->SetPosition({0.0f, 0.0f, 1.0f});
    grandchild->SetOrientation(grandchildOrientation);
    grandchild->SetScale({0.5f, 0.5f, 0.5f});

    const auto expectedGlobalTransform =
        parent->GetLocalTransform() * child->GetLocalTransform() * grandchild->GetLocalTransform();
    const auto& globalTransform = grandchild->GetGlobalTransform();
    for (glm::length_t column = 0; column < 4; ++column)
    {
        for (glm::length_t row = 0; row < 4; ++row)
        {
            EXPECT_NEAR(globalTransform[column][row], expectedGlobalTransform[column][row], 0.0001f);
        }
    }

    const auto expectedPosition = glm::vec3(expectedGlobalTransform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    const auto globalPosition = grandchild->GetGlobalPosition();
    EXPECT_NEAR(globalPosition.x, expectedPosition.x, 0.0001f);
    EXPECT_NEAR(globalPosition.y, expectedPosition.y, 0.0001f);
    EXPECT_NEAR(globalPosition.z, expectedPosition.z, 0.0001f);

    const auto expectedScale = glm::vec3(
        glm::length(glm::vec3(expectedGlobalTransform[0])),
        glm::length(glm::vec3(expectedGlobalTransform[1])),
        glm::length(glm::vec3(expectedGlobalTransform[2])));
    const auto globalScale = grandchild->GetGlobalScale();
    EXPECT_NEAR(globalScale.x, expectedScale.x, 0.0001f);
    EXPECT_NEAR(globalScale.y, expectedScale.y, 0.0001f);
    EXPECT_NEAR(globalScale.z, expectedScale.z, 0.0001f);

    const auto expectedOrientation = parentOrientation * childOrientation * grandchildOrientation;
    const auto globalOrientation = grandchild->GetGlobalOrientation();
    EXPECT_NEAR(std::abs(glm::dot(globalOrientation, expectedOrientation)), 1.0f, 0.0001f);
}

TEST_F(SceneNodeTest, GlobalTransformReflectsAncestorChangesAfterDescendantQuery)
{
    auto parent = scene->EmplaceChild<SceneNode3D>("Parent");
    parent->SetPosition({2.0f, 0.0f, 0.0f});
    auto child = parent->EmplaceChild<SceneNode3D>("Child");
    child->SetPosition({1.0f, 0.0f, 0.0f});
    auto grandchild = child->EmplaceChild<SceneNode3D>("Grandchild");
    grandchild->SetPosition({1.0f, 0.0f, 0.0f});

    const auto initialPosition = grandchild->GetGlobalPosition();
    EXPECT_NEAR(initialPosition.x, 4.0f, 0.0001f);

    parent->SetPosition({10.0f, 0.0f, 0.0f});

    const auto expectedGlobalTransform =
        parent->GetLocalTransform() * child->GetLocalTransform() * grandchild->GetLocalTransform();
    const auto& globalTransform = grandchild->GetGlobalTransform();
    for (glm::length_t column = 0; column < 4; ++column)
    {
        for (glm::length_t row = 0; row < 4; ++row)
        {
            EXPECT_NEAR(globalTransform[column][row], expectedGlobalTransform[column][row], 0.0001f);
        }
    }

    const auto updatedPosition = grandchild->GetGlobalPosition();
    EXPECT_NEAR(updatedPosition.x, 12.0f, 0.0001f);
    EXPECT_NEAR(updatedPosition.y, 0.0f, 0.0001f);
    EXPECT_NEAR(updatedPosition.z, 0.0f, 0.0001f);
}

TEST_F(SceneNodeTest, LocalTransformUsesOnlyComponentsPresentOnNodeType)
{
    auto plainNode = scene->EmplaceChild<SceneNode>("Plain Node");
    EXPECT_EQ(plainNode->GetLocalTransform(), glm::mat4(1.0f));

    auto parent = plainNode->EmplaceChild<SceneNode3D>("Transformed Parent");
    parent->SetPosition({10.0f, 0.0f, 0.0f});
    parent->SetOrientation(glm::quat(glm::radians(glm::vec3(0.0f, 90.0f, 0.0f))));
    auto light = parent->EmplaceChild<PointLight>("Point Light");
    light->SetPosition({1.0f, 2.0f, 3.0f});
    const auto lightLocalPosition = light->GetLocalTransform() * glm::vec4(0, 0, 0, 1);
    EXPECT_EQ(glm::vec3(lightLocalPosition), glm::vec3(1.0f, 2.0f, 3.0f));
    const auto lightGlobalPosition = light->GetGlobalPosition();
    EXPECT_NEAR(lightGlobalPosition.x, 13.0f, 0.0001f);
    EXPECT_NEAR(lightGlobalPosition.y, 2.0f, 0.0001f);
    EXPECT_NEAR(lightGlobalPosition.z, -1.0f, 0.0001f);

    auto camera = parent->EmplaceChild<Camera>("Camera");
    camera->SetPosition({4.0f, 5.0f, 6.0f});
    const auto cameraLocalPosition = camera->GetLocalTransform() * glm::vec4(0, 0, 0, 1);
    EXPECT_EQ(glm::vec3(cameraLocalPosition), glm::vec3(4.0f, 5.0f, 6.0f));
}

TEST_F(SceneNodeTest, ReparentingAndDetachingUpdatesGlobalTransform)
{
    auto firstParent = scene->EmplaceChild<SceneNode3D>("First Parent");
    firstParent->SetPosition({10.0f, 0.0f, 0.0f});
    auto secondParent = scene->EmplaceChild<SceneNode3D>("Second Parent");
    secondParent->SetPosition({20.0f, 0.0f, 0.0f});
    auto child = firstParent->EmplaceChild<SceneNode3D>("Child");
    child->SetPosition({1.0f, 0.0f, 0.0f});

    EXPECT_NEAR(child->GetGlobalPosition().x, 11.0f, 0.0001f);

    secondParent->AddChild(child);
    EXPECT_EQ(firstParent->GetNumChildren(), 0u);
    EXPECT_NEAR(child->GetGlobalPosition().x, 21.0f, 0.0001f);

    secondParent->RemoveChild(child);
    EXPECT_EQ(child->GetParent(), nullptr);
    EXPECT_NEAR(child->GetGlobalPosition().x, 1.0f, 0.0001f);
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

TEST_F(SceneNodeTest, GetAllDescendantsOfTypeReturnsOrderedMatches)
{
    auto first = scene->EmplaceChild<SceneNode3D>("First");
    auto plain = first->EmplaceChild<SceneNode>("Plain");
    auto nested = plain->EmplaceChild<SceneNode3D>("Nested");
    auto second = scene->EmplaceChild<SceneNode3D>("Second");

    const auto parentFirst = scene->GetAllDescendantsOfType<SceneNode3D>(true);
    ASSERT_EQ(parentFirst.size(), 3u);
    EXPECT_EQ(parentFirst[0], first.get());
    EXPECT_EQ(parentFirst[1], nested.get());
    EXPECT_EQ(parentFirst[2], second.get());

    const auto childFirst = scene->GetAllDescendantsOfType<SceneNode3D>(false);
    ASSERT_EQ(childFirst.size(), 3u);
    EXPECT_EQ(childFirst[0], nested.get());
    EXPECT_EQ(childFirst[1], first.get());
    EXPECT_EQ(childFirst[2], second.get());
}

TEST_F(SceneNodeTest, FindNodeById)
{
    auto node = scene->EmplaceChild<SceneNode>("Test Node");
    auto found = scene->FindChild(node->GetId(), true);
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