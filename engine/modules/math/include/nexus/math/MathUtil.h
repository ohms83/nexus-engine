#pragma once

#include "glm/glm.hpp"

#include <format>
#include <string>

namespace nxs
{
    namespace Math
    {
        std::string ToString(glm::vec2 vec);
        std::string ToString(glm::vec3 vec);
        std::string ToString(glm::mat3 mat);
        std::string ToString(glm::mat4 mat);
    }
}