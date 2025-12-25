#include "nexus/Nexus.h"
#include "gtest/gtest.h"

using namespace nxs;

TEST(RenderCommandBatcher, BatchesAdjacentSameDraw)
{
    RenderCommand a, b, c;
    // Mock vertex/index buffers and materials
    a.vertexBuffer = nullptr; b.vertexBuffer = nullptr; c.vertexBuffer = nullptr;
    a.indexBuffer = nullptr; b.indexBuffer = nullptr; c.indexBuffer = nullptr;


    auto matA = std::make_shared<Material>("mA", 1u);
    a.material = matA; b.material = matA; c.material = std::make_shared<Material>("mC", 2u);

    glm::mat4 m1(1.0f), m2(1.0f), m3(1.0f);
    a.modelMatrix = &m1;
    b.modelMatrix = &m2;
    c.modelMatrix = &m3;

    a.gpuProgram = nullptr; b.gpuProgram = nullptr; c.gpuProgram = nullptr;

    std::vector<RenderCommand> inputs{a, b, c};
    // Test both batch functions
    RenderCommandBatcher::Batch(inputs);
    // First two are batchable, third is separate
    EXPECT_EQ(inputs.size(), 2);
    // Check that first output has instanceModels size 2 and second output has instanceModels 0 (single)
    EXPECT_EQ(inputs[0].instanceModels.size(), 2);
    EXPECT_TRUE(inputs[1].instanceModels.empty());
    // In-place batch
}
