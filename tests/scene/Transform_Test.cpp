#include "gtest/gtest.h"
#include "nexus/Nexus.h"
#include <iostream>
#include <sstream>
#include <format>
#include <string>
#include <iomanip>

std::string FormatVector(const glm::vec3& vec)
{
    return std::format("({}, {}, {}) Length={}", vec.x, vec.y, vec.z, glm::length(vec));
}

TEST(TransformTest, ForwardVector) {
    const float epsilon = 0.00001f; // A small tolerance value
    const int iteration = 10; // Try testing with the 10 random data.
    
    for (int i = 0; i < iteration; ++i)
    {
        const glm::vec3 randPos = glm::ballRand(100.f);
        const glm::vec3 randTarget = glm::ballRand(100.f);
        const glm::vec3 up {0, 1, 0};
        
        nxs::Transform transform;
        transform.SetPosition(randPos);
        transform.LookAt(randTarget, up);

        const glm::vec3 forward1 = glm::normalize(randTarget - randPos);
        const glm::vec3 forward2 = transform.Forward();
        const glm::vec3 diff = glm::abs(forward1 - forward2);
        const auto results = glm::equal(forward1, forward2, epsilon);

        const auto errorMessage = std::format("forward1={} forward2={} diff={}",
            FormatVector(forward1), FormatVector(forward2), FormatVector(diff));
        EXPECT_TRUE(glm::all(results)) << errorMessage;
    }
}

TEST(TransformTest, RightVector) {
    const float epsilon = 0.00001f; // A small tolerance value
    const int iteration = 10; // Try testing with the 10 random data.
    
    for (int i = 0; i < iteration; ++i)
    {
        const glm::vec3 randPos = glm::ballRand(100.f);
        const glm::vec3 randTarget = glm::ballRand(100.f);
        const glm::vec3 up {0, 1, 0};
        
        nxs::Transform transform;
        transform.SetPosition(randPos);
        transform.LookAt(randTarget, up);

        const glm::vec3 f = glm::normalize(randTarget - randPos);
        const glm::vec3 right1 = glm::normalize(glm::cross(f, up));
        const glm::vec3 right2 = transform.Right();
        const glm::vec3 diff = glm::abs(right1 - right2);
        const auto results = glm::equal(right1, right2, epsilon);

        const auto errorMessage = std::format("right1={} right2={} diff={}",
            FormatVector(right1), FormatVector(right2), FormatVector(diff));
        EXPECT_TRUE(glm::all(results)) << errorMessage;
    }
}

TEST(TransformTest, UpVector) {
    const float epsilon = 0.00001f; // A small tolerance value
    const int iteration = 1; // Try testing with the 10 random data.
    
    for (int i = 0; i < iteration; ++i)
    {
        const glm::vec3 randPos = glm::ballRand(100.f);
        const glm::vec3 randTarget = glm::ballRand(100.f);
        const glm::vec3 up {0, 1, 0};
        
        nxs::Transform transform;
        transform.SetPosition(randPos);
        transform.LookAt(randTarget, up);

        const glm::vec3 f = glm::normalize(randTarget - randPos);
        const glm::vec3 s = glm::normalize(glm::cross(f, up));
        const glm::vec3 up1 = glm::normalize(glm::cross(s, f));
        const glm::vec3 up2 = transform.Up();
        const glm::vec3 diff = glm::abs(up1 - up2);
        const auto results = glm::equal(up1, up2, epsilon);

        const auto errorMessage = std::format("up1={} up2={} diff={}",
            FormatVector(up1), FormatVector(up2), FormatVector(diff));
        EXPECT_TRUE(glm::all(results)) << errorMessage;
    }
}

TEST(TransformTest, CalculateViewMatrix) {
    const float epsilon = 0.0001f; // A small tolerance value
    const int iteration = 10; // Try testing with the 10 random data.

    for (int i = 0; i < iteration; ++i)
    {
        const glm::vec3 randPos = glm::ballRand(100.f);
        const glm::vec3 randTarget = glm::ballRand(100.f);
        const glm::vec3 up {0, 1, 0};
        
        glm::quat rotation = glm::quatLookAt(glm::normalize(randTarget - randPos), up);
        glm::mat4 cameraView = nxs::Matrix::CreateViewMatrix(randPos, rotation);

        glm::mat4 glmView = glm::lookAt(randPos, randTarget, up);
        glm::bvec4 result_columns = glm::equal(glmView, cameraView, epsilon);

        std::stringstream errorMessage;

        errorMessage << "Matrices are NOT approximately equal within epsilon (" << epsilon << ")" << std::endl;
        errorMessage << "Column comparison results (mat1 vs mat2): " << std::endl;
        for (int i = 0; i < 4; ++i) {
            errorMessage << "  Column " << i << ": " << (result_columns[i] ? "True" : "False") << std::endl;
            for (int j = 0; j < 4; ++j) {
                errorMessage << std::fixed << std::setprecision(8)
                        << "    mat1[" << i << "][" << j << "] = " << glmView[i][j]
                        << ", mat2[" << i << "][" << j << "] = " << cameraView[i][j]
                        << ", diff = " << std::abs(glmView[i][j] - cameraView[i][j]) << std::endl;
            }
        }

        errorMessage << "randPos = " << FormatVector(randPos) << std::endl;
        errorMessage << "randTarget = " << FormatVector(randTarget) << std::endl;

        EXPECT_TRUE(glm::all(result_columns)) << errorMessage.str();
    }
}