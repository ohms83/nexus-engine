#pragma once

#include "nexus/NxsCommon.h"

#include <format>
#include <string>

NXS_NAMESPACE
{
    namespace math
    {
        std::string ToString(glm::vec2 vec);
        std::string ToString(glm::vec3 vec);
        std::string ToString(glm::mat3 mat);
        std::string ToString(glm::mat4 mat);
    }
}