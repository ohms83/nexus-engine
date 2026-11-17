#pragma once

#include "glm/glm.hpp"
#include <glm/gtc/quaternion.hpp>
#include "glm/gtx/quaternion.hpp"

namespace nxs
{
    namespace Quaternion
    {
        inline glm::vec3 GetEulerAngles(const glm::quat quat)
        {
            return glm::degrees(glm::eulerAngles(quat));
        }

        inline glm::quat FromEulerAngles(const glm::vec3& angles)
        {
            return glm::quat(glm::radians(angles));
        }
    }
}