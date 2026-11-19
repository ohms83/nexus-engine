#include "gtest/gtest.h"
#include "nexus/Nexus.h"
#include <sstream>
#include <format>
#include <string>
#include <iomanip>

#include "helpers/FakeReflector.h"

namespace
{
    class TransformTestReflector final : public FakeReflector
    {
    public:
        void SetReadOnlyFlag(bool value) override
        {
            disabledDepth += value ? 1 : -1;
            EXPECT_GE(disabledDepth, 0);
        }
        int disabledDepth = 0;

    private:
        bool VisitVec3_Implement(const std::string& name, glm::vec3& value) override
        {
            if (disabledDepth != 0 || name != "Position") return false;
            value = {4.0f, 5.0f, 6.0f};
            return true;
        }
    };
}

TEST(TransformComponentTest, ReflectionAndDeserializationInvalidateCachedTransforms)
{
    auto registry = std::make_shared<entt::registry>();
    auto parent = std::make_shared<nxs::SceneNode>(registry);
    auto child = std::make_shared<nxs::SceneNode>(registry);
    parent->AddChild(child);
    EXPECT_EQ(child->GetGlobalPosition(), glm::vec3(0.0f));

    TransformTestReflector reflector;
    auto transform = parent->GetComponent<nxs::TransformComponent>();
    transform->AcceptReflector(reflector);
    EXPECT_EQ(reflector.disabledDepth, 0);
    EXPECT_EQ(child->GetGlobalPosition(), glm::vec3(4.0f, 5.0f, 6.0f));

    nxs::TransformComponent replacement;
    replacement.SetPosition({7.0f, 8.0f, 9.0f});
    ASSERT_TRUE(transform->Deserialize(replacement.Serialize()));
    EXPECT_EQ(child->GetGlobalPosition(), glm::vec3(7.0f, 8.0f, 9.0f));
}

TEST(TransformComponentTest, DefaultsAreNeutralAndValid)
{
    const nxs::TransformComponent transform;
    EXPECT_EQ(transform.GetPosition(), glm::vec3(0.0f));
    EXPECT_EQ(transform.GetQuaternion(), glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
    EXPECT_EQ(transform.GetAngles(), glm::vec3(0.0f));
    EXPECT_EQ(transform.GetScale(), glm::vec3(1.0f));

    const nxs::MoveComponent move;
    EXPECT_EQ(move.direction, glm::vec3(0.0f));
    EXPECT_FLOAT_EQ(move.speed, 0.0f);

    const nxs::TurningComponent turning;
    EXPECT_EQ(turning.axis, glm::vec3(0.0f, 1.0f, 0.0f));
    EXPECT_FLOAT_EQ(turning.degree, 0.0f);
}

TEST(TransformComponentTest, SceneNodeOrientationRoundTripsThroughSerialization)
{
    nxs::SceneNode node(std::make_shared<entt::registry>());
    const glm::quat rotation = glm::quat(glm::radians(glm::vec3(20.0f, 35.0f, -10.0f)));
    node.SetPosition({1.0f, 2.0f, 3.0f});
    node.SetScale({4.0f, 5.0f, 6.0f});
    node.SetOrientation(rotation);

    const auto* orientation = node.GetComponent<nxs::TransformComponent>();
    ASSERT_NE(orientation, nullptr);
    EXPECT_EQ(node.GetOrientation(), rotation);
    EXPECT_TRUE(glm::all(glm::epsilonEqual(
        orientation->GetAngles(), glm::vec3(20.0f, 35.0f, -10.0f), 0.001f)));

    nxs::TransformComponent restored;
    ASSERT_TRUE(restored.Deserialize(orientation->Serialize()));
    EXPECT_TRUE(glm::all(glm::epsilonEqual(restored.GetQuaternion(), rotation, 0.00001f)));
    EXPECT_EQ(restored.GetPosition(), node.GetPosition());
    EXPECT_EQ(restored.GetScale(), node.GetScale());
    EXPECT_EQ(restored.GetMatrix(), nxs::Matrix::ExpandAffine(restored.GetAffine()));
}

TEST(TransformComponentTest, PlaneMeshSerializationPreservesBaseFields)
{
    nxs::PlaneMeshComponent plane;
    plane.meshPath = "plane";
    plane.showBoundingBox = true;
    plane.width = 3.0f;
    plane.height = 5.0f;

    nxs::PlaneMeshComponent restored;
    ASSERT_TRUE(restored.Deserialize(plane.Serialize()));
    EXPECT_EQ(restored.meshPath, "plane");
    EXPECT_TRUE(restored.showBoundingBox);
    EXPECT_FLOAT_EQ(restored.width, 3.0f);
    EXPECT_FLOAT_EQ(restored.height, 5.0f);
}

std::string FormatVector(const glm::vec3& vec)
{
    return std::format("({}, {}, {}) Length={}", vec.x, vec.y, vec.z, glm::length(vec));
}

TEST(SceneNode3DTest, ForwardVector) {
    constexpr int iteration = 10; // Try testing with the 10 random data.

    for (int i = 0; i < iteration; ++i)
    {
        constexpr float epsilon = 0.00001f;
        const glm::vec3 randPos = glm::ballRand(100.f);
        const glm::vec3 randTarget = glm::ballRand(100.f);
        constexpr glm::vec3 up {0, 1, 0};

        nxs::SceneNode node(std::make_shared<entt::registry>());
        node.SetPosition(randPos);
        node.LookAt(randTarget, up);

        const glm::vec3 forward1 = glm::normalize(randTarget - randPos);
        const glm::vec3 forward2 = node.Forward();
        const glm::vec3 diff = glm::abs(forward1 - forward2);
        const auto results = glm::equal(forward1, forward2, epsilon);

        const auto errorMessage = std::format("forward1={} forward2={} diff={}",
            FormatVector(forward1), FormatVector(forward2), FormatVector(diff));
        EXPECT_TRUE(glm::all(results)) << errorMessage;
    }
}

TEST(SceneNode3DTest, RightVector) {
    constexpr int iteration = 10; // Try testing with the 10 random data.

    for (int i = 0; i < iteration; ++i)
    {
        constexpr float epsilon = 0.00001f;
        const glm::vec3 randPos = glm::ballRand(100.f);
        const glm::vec3 randTarget = glm::ballRand(100.f);
        constexpr glm::vec3 up {0, 1, 0};
        
        nxs::SceneNode node(std::make_shared<entt::registry>());
        node.SetPosition(randPos);
        node.LookAt(randTarget, up);

        const glm::vec3 f = glm::normalize(randTarget - randPos);
        const glm::vec3 right1 = glm::normalize(glm::cross(f, up));
        const glm::vec3 right2 = node.Right();
        const glm::vec3 diff = glm::abs(right1 - right2);
        const auto results = glm::equal(right1, right2, epsilon);

        const auto errorMessage = std::format("right1={} right2={} diff={}",
            FormatVector(right1), FormatVector(right2), FormatVector(diff));
        EXPECT_TRUE(glm::all(results)) << errorMessage;
    }
}

TEST(SceneNode3DTest, UpVector) {
    constexpr int iteration = 10; // Try testing with the 10 random data.
    
    for (int i = 0; i < iteration; ++i)
    {
        constexpr float epsilon = 0.00001f;
        const glm::vec3 randPos = glm::ballRand(100.f);
        const glm::vec3 randTarget = glm::ballRand(100.f);
        constexpr glm::vec3 up {0, 1, 0};
        
        nxs::SceneNode node(std::make_shared<entt::registry>());
        node.SetPosition(randPos);
        node.LookAt(randTarget, up);

        const glm::vec3 f = glm::normalize(randTarget - randPos);
        const glm::vec3 s = glm::normalize(glm::cross(f, up));
        const glm::vec3 up1 = glm::normalize(glm::cross(s, f));
        const glm::vec3 up2 = node.Up();
        const glm::vec3 diff = glm::abs(up1 - up2);
        const auto results = glm::equal(up1, up2, epsilon);

        const auto errorMessage = std::format("up1={} up2={} diff={}",
            FormatVector(up1), FormatVector(up2), FormatVector(diff));
        EXPECT_TRUE(glm::all(results)) << errorMessage;
    }
}

TEST(TransformTest, CalculateViewMatrix) {
    constexpr int iteration = 10; // Try testing with the 10 random data.

    for (int i = 0; i < iteration; ++i)
    {
        constexpr float epsilon = 0.0001f;
        const glm::vec3 randPos = glm::ballRand(100.f);
        const glm::vec3 randTarget = glm::ballRand(100.f);
        constexpr glm::vec3 up {0, 1, 0};
        
        glm::quat rotation = glm::quatLookAt(glm::normalize(randTarget - randPos), up);
        glm::mat4 cameraView = nxs::Matrix::CreateViewMatrix(randPos, rotation);

        glm::mat4 glmView = glm::lookAt(randPos, randTarget, up);
        glm::bvec4 result_columns = glm::equal(glmView, cameraView, epsilon);

        std::stringstream errorMessage;

        errorMessage << "Matrices are NOT approximately equal within epsilon (" << epsilon << ")" << std::endl;
        errorMessage << "Column comparison results (mat1 vs mat2): " << std::endl;
        for (int j = 0; j < 4; ++j) {
            errorMessage << "  Column " << j << ": " << (result_columns[j] ? "True" : "False") << std::endl;
            for (int k = 0; k < 4; ++k) {
                errorMessage << std::fixed << std::setprecision(8)
                        << "    mat1[" << j << "][" << k << "] = " << glmView[j][k]
                        << ", mat2[" << j << "][" << k << "] = " << cameraView[j][k]
                        << ", diff = " << std::abs(glmView[j][k] - cameraView[j][k]) << std::endl;
            }
        }

        errorMessage << "randPos = " << FormatVector(randPos) << std::endl;
        errorMessage << "randTarget = " << FormatVector(randTarget) << std::endl;

        EXPECT_TRUE(glm::all(result_columns)) << errorMessage.str();
    }
}