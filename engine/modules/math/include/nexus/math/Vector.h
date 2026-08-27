#pragma once

#include "glm/glm.hpp"
#include "Math.h"

namespace nxs
{
    namespace Vector
    {
        inline glm::vec2 SafeNormalize(glm::vec2 vec)
        {
            return Math::AlmostZero(glm::length2(vec)) ? glm::vec2(0) : glm::normalize(vec);
        }

        inline glm::vec3 SafeNormalize(glm::vec3 vec)
        {
            return Math::AlmostZero(glm::length2(vec)) ? glm::vec3(0) : glm::normalize(vec);
        }

        inline glm::vec4 SafeNormalize(glm::vec4 vec)
        {
            return Math::AlmostZero(glm::length2(vec)) ? glm::vec4(0) : glm::normalize(vec);
        }
    }
}