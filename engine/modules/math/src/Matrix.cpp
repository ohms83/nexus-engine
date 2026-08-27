//
// Created by nutta on 7/25/2025.
//

#include "nexus/math/Matrix.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"

using namespace nxs;

glm::mat4 Matrix::CreateModelMatrix(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale)
{
    auto modelMatrix = glm::mat4(1.0f);
    // Apply Translation
    modelMatrix = glm::translate(modelMatrix, position);
    // Apply Rotation
    modelMatrix = modelMatrix * glm::mat4_cast(rotation);
    // Apply Scaling
    modelMatrix = glm::scale(modelMatrix, scale);
    return modelMatrix;
}

glm::mat4 Matrix::CreateViewMatrix(const glm::vec3& position, const glm::quat& rotation)
{
    auto viewMatrix = glm::mat4(rotation);
    viewMatrix[3] = glm::vec4(position, 1);
    return glm::inverse(viewMatrix);
}
