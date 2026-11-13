#include "gtest/gtest.h"
#include "nexus/Nexus.h"

USING_NAMESPACE_NXS;

static Frustum MakeTestFrustum(const glm::vec3& minCorner, const glm::vec3& maxCorner)
{
    Frustum frustum;
    // Left: x >= minCorner.x
    frustum.planes[0] = Plane(glm::vec3(1, 0, 0), glm::vec3(minCorner.x, 0, 0));
    // Right: x <= maxCorner.x
    frustum.planes[1] = Plane(glm::vec3(-1, 0, 0), glm::vec3(maxCorner.x, 0, 0));
    // Bottom: y >= minCorner.y
    frustum.planes[2] = Plane(glm::vec3(0, 1, 0), glm::vec3(0, minCorner.y, 0));
    // Top: y <= maxCorner.y
    frustum.planes[3] = Plane(glm::vec3(0, -1, 0), glm::vec3(0, maxCorner.y, 0));
    // Near: z >= minCorner.z
    frustum.planes[4] = Plane(glm::vec3(0, 0, 1), glm::vec3(0, 0, minCorner.z));
    // Far: z <= maxCorner.z
    frustum.planes[5] = Plane(glm::vec3(0, 0, -1), glm::vec3(0, 0, maxCorner.z));
    return frustum;
}

class QuadtreeTest : public ::testing::Test {
protected:
    Ref<Scene> scene;

    void SetUp() override {
        scene = std::make_shared<Scene>("Test Scene");
    }

    void TearDown() override {
        scene.reset();
    }
};

TEST_F(QuadtreeTest, EmptyTreeQueryReturnsNoResults)
{
    Quadtree tree(4, 2, Box(glm::vec3(0.0f), glm::vec3(100.0f)));
    Array<SceneNode*> results;
    Frustum frustum = MakeTestFrustum(glm::vec3(-50.0f), glm::vec3(50.0f));

    tree.Query(frustum, results);
    EXPECT_TRUE(results.empty());
}

TEST_F(QuadtreeTest, SingleNodeInsideFrustumIsReturned)
{
    Quadtree tree(4, 2, Box(glm::vec3(0.0f), glm::vec3(100.0f)));
    auto node = scene->EmplaceChild<SceneNode3D>("NodeA");

    tree.AddNode(node, Box(glm::vec3(10.0f, 0.0f, 10.0f), glm::vec3(2.0f)));

    Array<SceneNode*> results;
    Frustum frustum = MakeTestFrustum(glm::vec3(0.0f, -10.0f, 0.0f), glm::vec3(20.0f, 10.0f, 20.0f));

    tree.Query(frustum, results);
    ASSERT_EQ(results.size(), 1u);
    EXPECT_EQ(results[0], node.get());
}

TEST_F(QuadtreeTest, SingleNodeOutsideFrustumIsCulled)
{
    Quadtree tree(4, 2, Box(glm::vec3(0.0f), glm::vec3(100.0f)));
    auto node = scene->EmplaceChild<SceneNode3D>("NodeA");

    tree.AddNode(node, Box(glm::vec3(80.0f, 0.0f, 80.0f), glm::vec3(2.0f)));

    Array<SceneNode*> results;
    Frustum frustum = MakeTestFrustum(glm::vec3(-50.0f), glm::vec3(0.0f));

    tree.Query(frustum, results);
    EXPECT_TRUE(results.empty());
}

TEST_F(QuadtreeTest, InactiveNodeIsCulled)
{
    Quadtree tree(4, 2, Box(glm::vec3(0.0f), glm::vec3(100.0f)));
    auto node = scene->EmplaceChild<SceneNode3D>("NodeA");
    node->Activate(false);

    tree.AddNode(node, Box(glm::vec3(10.0f, 0.0f, 10.0f), glm::vec3(2.0f)));

    Array<SceneNode*> results;
    Frustum frustum = MakeTestFrustum(glm::vec3(-50.0f), glm::vec3(50.0f));

    tree.Query(frustum, results);
    EXPECT_TRUE(results.empty());
}

TEST_F(QuadtreeTest, SubdivisionOnCapacityExceeded)
{
    // maxDepth = 2, partitionSize = 2, root bound = [-100, 100]
    Quadtree tree(2, 2, Box(glm::vec3(0.0f), glm::vec3(100.0f)));

    auto node1 = scene->EmplaceChild<SceneNode3D>("Node1");
    auto node2 = scene->EmplaceChild<SceneNode3D>("Node2");
    auto node3 = scene->EmplaceChild<SceneNode3D>("Node3");

    // Place them in different quadrants:
    // PlusXPlusZ: center at (50, 0, 50)
    tree.AddNode(node1, Box(glm::vec3(50.0f, 0.0f, 50.0f), glm::vec3(5.0f)));
    // MinusXMinusZ: center at (-50, 0, -50)
    tree.AddNode(node2, Box(glm::vec3(-50.0f, 0.0f, -50.0f), glm::vec3(5.0f)));

    // Before exceeding partitionSize, no quadrants
    EXPECT_EQ(tree.GetQuadrants().size(), 0u);
    EXPECT_EQ(tree.GetNodeCount(), 2u);

    // Adding 3rd node triggers subdivision
    // MinusXPlusZ: center at (-50, 0, 50)
    tree.AddNode(node3, Box(glm::vec3(-50.0f, 0.0f, 50.0f), glm::vec3(5.0f)));

    EXPECT_EQ(tree.GetQuadrants().size(), 4u);
    EXPECT_EQ(tree.GetNodeCount(), 0u); // Parent nodeList cleared after subdivision

    // Query only the PlusXPlusZ quadrant
    Array<SceneNode*> results;
    Frustum frustum = MakeTestFrustum(glm::vec3(20.0f, -50.0f, 20.0f), glm::vec3(80.0f, 50.0f, 80.0f));
    tree.Query(frustum, results);

    ASSERT_EQ(results.size(), 1u);
    EXPECT_EQ(results[0], node1.get());
}

TEST_F(QuadtreeTest, MaxDepthPreventsInfiniteSubdivision)
{
    // maxDepth = 1, partitionSize = 2
    Quadtree tree(1, 2, Box(glm::vec3(0.0f), glm::vec3(100.0f)));

    // Add 4 nodes into the same quadrant (+X, +Z)
    auto node1 = scene->EmplaceChild<SceneNode3D>("Node1");
    auto node2 = scene->EmplaceChild<SceneNode3D>("Node2");
    auto node3 = scene->EmplaceChild<SceneNode3D>("Node3");
    auto node4 = scene->EmplaceChild<SceneNode3D>("Node4");

    tree.AddNode(node1, Box(glm::vec3(50.0f, 0.0f, 50.0f), glm::vec3(2.0f)));
    tree.AddNode(node2, Box(glm::vec3(51.0f, 0.0f, 51.0f), glm::vec3(2.0f)));
    tree.AddNode(node3, Box(glm::vec3(52.0f, 0.0f, 52.0f), glm::vec3(2.0f)));
    tree.AddNode(node4, Box(glm::vec3(53.0f, 0.0f, 53.0f), glm::vec3(2.0f)));

    // Root should have subdivided to depth 1
    ASSERT_EQ(tree.GetQuadrants().size(), 4u);

    // Quadrant 0 (PlusXPlusZ) at depth 1 should have reached maxDepth (1),
    // so it should NOT subdivide further even with > partitionSize nodes.
    const auto& quad0 = tree.GetQuadrants()[0];
    EXPECT_EQ(quad0->GetDepth(), 1u);
    EXPECT_EQ(quad0->GetQuadrants().size(), 0u);
    EXPECT_EQ(quad0->GetNodeCount(), 4u);
}

TEST_F(QuadtreeTest, SpanningNodeDoesNotProduceDuplicateQueryResults)
{
    Quadtree tree(2, 2, Box(glm::vec3(0.0f), glm::vec3(100.0f)));

    // Create 3 nodes so the tree subdivides
    auto nodeA = scene->EmplaceChild<SceneNode3D>("NodeA");
    auto nodeB = scene->EmplaceChild<SceneNode3D>("NodeB");
    // Node spanning the origin (0,0,0), overlapping all 4 quadrants
    auto nodeSpanning = scene->EmplaceChild<SceneNode3D>("NodeSpanning");

    tree.AddNode(nodeA, Box(glm::vec3(60.0f, 0.0f, 60.0f), glm::vec3(5.0f)));
    tree.AddNode(nodeB, Box(glm::vec3(-60.0f, 0.0f, -60.0f), glm::vec3(5.0f)));
    tree.AddNode(nodeSpanning, Box(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(15.0f)));

    EXPECT_EQ(tree.GetQuadrants().size(), 4u);

    // Query with a large frustum that intersects all 4 quadrants
    Array<SceneNode*> results;
    Frustum frustum = MakeTestFrustum(glm::vec3(-80.0f), glm::vec3(80.0f));
    tree.Query(frustum, results);

    // All 3 nodes must be present, and nodeSpanning must appear EXACTLY ONCE
    EXPECT_EQ(results.size(), 3u);

    size_t spanningCount = 0;
    for (auto* n : results)
    {
        if (n == nodeSpanning.get()) ++spanningCount;
    }
    EXPECT_EQ(spanningCount, 1u);
}

TEST_F(QuadtreeTest, RemoveNodeRemovesFromLeavesAndQuadrants)
{
    Quadtree tree(2, 2, Box(glm::vec3(0.0f), glm::vec3(100.0f)));

    auto node1 = scene->EmplaceChild<SceneNode3D>("Node1");
    auto node2 = scene->EmplaceChild<SceneNode3D>("Node2");
    auto node3 = scene->EmplaceChild<SceneNode3D>("Node3");

    tree.AddNode(node1, Box(glm::vec3(50.0f, 0.0f, 50.0f), glm::vec3(5.0f)));
    tree.AddNode(node2, Box(glm::vec3(-50.0f, 0.0f, -50.0f), glm::vec3(5.0f)));
    tree.AddNode(node3, Box(glm::vec3(50.0f, 0.0f, -50.0f), glm::vec3(5.0f)));

    // Verify all 3 returned initially
    Array<SceneNode*> results;
    Frustum frustum = MakeTestFrustum(glm::vec3(-100.0f), glm::vec3(100.0f));
    tree.Query(frustum, results);
    EXPECT_EQ(results.size(), 3u);

    // Remove node2
    tree.RemoveNode(node2->GetId());

    results.clear();
    tree.Query(frustum, results);
    EXPECT_EQ(results.size(), 2u);

    for (auto* n : results)
    {
        EXPECT_NE(n, node2.get());
    }
}

TEST_F(QuadtreeTest, UpdateNodeMovesNodeToNewQuadrant)
{
    Quadtree tree(2, 2, Box(glm::vec3(0.0f), glm::vec3(100.0f)));

    auto node1 = scene->EmplaceChild<SceneNode3D>("Node1");
    auto node2 = scene->EmplaceChild<SceneNode3D>("Node2");
    auto node3 = scene->EmplaceChild<SceneNode3D>("Node3");

    tree.AddNode(node1, Box(glm::vec3(50.0f, 0.0f, 50.0f), glm::vec3(5.0f)));
    tree.AddNode(node2, Box(glm::vec3(-50.0f, 0.0f, -50.0f), glm::vec3(5.0f)));
    tree.AddNode(node3, Box(glm::vec3(50.0f, 0.0f, -50.0f), glm::vec3(5.0f)));

    // Move node2 from (-50, 0, -50) to (+70, 0, +70) in PlusXPlusZ
    tree.UpdateNode(node2->GetId(), Box(glm::vec3(70.0f, 0.0f, 70.0f), glm::vec3(5.0f)));

    // Query MinusXMinusZ: node2 should NO LONGER be there
    Array<SceneNode*> resultsMinus;
    Frustum frustumMinus = MakeTestFrustum(glm::vec3(-90.0f, -50.0f, -90.0f), glm::vec3(-10.0f, 50.0f, -10.0f));
    tree.Query(frustumMinus, resultsMinus);
    EXPECT_EQ(resultsMinus.size(), 0u);

    // Query PlusXPlusZ: both node1 and node2 should be there
    Array<SceneNode*> resultsPlus;
    Frustum frustumPlus = MakeTestFrustum(glm::vec3(10.0f, -50.0f, 10.0f), glm::vec3(90.0f, 50.0f, 90.0f));
    tree.Query(frustumPlus, resultsPlus);
    EXPECT_EQ(resultsPlus.size(), 2u);
}

TEST_F(QuadtreeTest, RemoveAllNodesResetsTree)
{
    Quadtree tree(2, 2, Box(glm::vec3(0.0f), glm::vec3(100.0f)));

    auto node1 = scene->EmplaceChild<SceneNode3D>("Node1");
    auto node2 = scene->EmplaceChild<SceneNode3D>("Node2");
    auto node3 = scene->EmplaceChild<SceneNode3D>("Node3");

    tree.AddNode(node1, Box(glm::vec3(50.0f, 0.0f, 50.0f), glm::vec3(5.0f)));
    tree.AddNode(node2, Box(glm::vec3(-50.0f, 0.0f, -50.0f), glm::vec3(5.0f)));
    tree.AddNode(node3, Box(glm::vec3(50.0f, 0.0f, -50.0f), glm::vec3(5.0f)));

    EXPECT_EQ(tree.GetQuadrants().size(), 4u);

    tree.RemoveAllNodes();
    EXPECT_EQ(tree.GetQuadrants().size(), 0u);
    EXPECT_EQ(tree.GetNodeCount(), 0u);

    Array<SceneNode*> results;
    Frustum frustum = MakeTestFrustum(glm::vec3(-100.0f), glm::vec3(100.0f));
    tree.Query(frustum, results);
    EXPECT_TRUE(results.empty());
}

TEST_F(QuadtreeTest, SceneCameraVisibilityCullingWithPartition)
{
    // Setup Scene with Quadtree partition
    auto partition = std::make_shared<Quadtree>(4, 2, Box(glm::vec3(0.0f), glm::vec3(1000.0f)));
    scene->SetPartition(partition);

    // Add nodes with meshes at various positions
    auto meshA = std::make_shared<Mesh>("MeshA");
    meshA->SetSphere(Sphere(glm::vec3(0.0f), 2.0f));

    auto nodeVisible = scene->EmplaceChild<SceneNode3D>("VisibleNode");
    nodeVisible->AddComponent<MeshComponent>()->SetMesh(meshA);
    nodeVisible->SetPosition(glm::vec3(0.0f, 0.0f, -50.0f));

    auto nodeBehindCamera = scene->EmplaceChild<SceneNode3D>("BehindNode");
    nodeBehindCamera->AddComponent<MeshComponent>()->SetMesh(meshA);
    nodeBehindCamera->SetPosition(glm::vec3(0.0f, 0.0f, 50.0f));

    auto nodeFarAway = scene->EmplaceChild<SceneNode3D>("FarAwayNode");
    nodeFarAway->AddComponent<MeshComponent>()->SetMesh(meshA);
    nodeFarAway->SetPosition(glm::vec3(500.0f, 0.0f, -50.0f));

    // Refresh partition so world bounds are up to date
    scene->RefreshPartition();

    // Create Camera at origin looking down -Z
    auto camera = scene->EmplaceChild<Camera>("MainCamera");
    camera->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    camera->LookAt(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    camera->SetProjection(60.0f, 800.0f, 600.0f, 0.1f, 200.0f);

    auto visibleNodes = camera->GetVisibleNodes(*scene);

    // Only nodeVisible should be visible
    ASSERT_EQ(visibleNodes.size(), 1u);
    EXPECT_EQ(visibleNodes[0], nodeVisible.get());
}
