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

        //! Expands an affine 4x3 transform to a 4x4 matrix with a (0, 0, 0, 1) bottom row.
        inline glm::mat4 ExpandAffine(const glm::mat4x3& transform)
        {
            return glm::mat4(
                glm::vec4(transform[0], 0.0f),
                glm::vec4(transform[1], 0.0f),
                glm::vec4(transform[2], 0.0f),
                glm::vec4(transform[3], 1.0f));
        }

        //! Composes two affine 4x3 transforms, returning parent * local.
        inline glm::mat4x3 ComposeAffine(const glm::mat4x3& parent, const glm::mat4x3& local)
        {
            glm::mat4x3 result(1.0f);
            result[0] = parent * glm::vec4(local[0], 0.0f);
            result[1] = parent * glm::vec4(local[1], 0.0f);
            result[2] = parent * glm::vec4(local[2], 0.0f);
            result[3] = parent * glm::vec4(local[3], 1.0f);
            return result;
        }
    };
}