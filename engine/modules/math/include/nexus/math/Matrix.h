//
// Created by nutta on 7/25/2025.
//

#pragma once

#include "glm/glm.hpp"

namespace nxs
{
    namespace Matrix
    {
        glm::mat4 CreateModelMatrix(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale);
        glm::mat4 CreateViewMatrix(const glm::vec3& position, const glm::quat& rotation);
    };
}