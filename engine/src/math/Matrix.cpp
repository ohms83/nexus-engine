//
// Created by nutta on 7/25/2025.
//

#include "nexus/math/Matrix.h"

USING_NAMESPACE_NXS;

glm::mat4 Matrix::CreateModelMatrix(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale)
{
    auto modelMatrix = glm::mat4(1.0f);
    // Apply Scaling
    modelMatrix = glm::scale(modelMatrix, scale);
    // Apply Rotation
    modelMatrix = glm::mat4_cast(rotation) * modelMatrix;
    // Apply Translation
    modelMatrix = glm::translate(modelMatrix, position);
    return modelMatrix;
}

glm::mat4 Matrix::CreateViewMatrix(const glm::vec3& position, const glm::quat& rotation)
{
    auto viewMatrix = glm::mat4(rotation);
    viewMatrix[3] = glm::vec4(position, 1);
    return glm::inverse(viewMatrix);
}
