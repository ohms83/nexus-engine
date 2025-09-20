#include "math/MathUtil.h"

USING_NAMESPACE_NXS;

std::string math::ToString(glm::vec2 vec)
{
    return std::format("({}, {})", vec.x, vec.y);
}
std::string math::ToString(glm::vec3 vec)
{
    return std::format("({}, {}, {})", vec.x, vec.y, vec.z);
}
std::string math::ToString(glm::mat3 mat)
{
    std::string result;
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            result += std::format("[{}][{}]={}\n", i, j, mat[i][j]);
        }
        result += "\n";
    }
    return result;
}
std::string math::ToString(glm::mat4 mat)
{
    std::string result;
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            result += std::format("[{}][{}]={}\n", i, j, mat[i][j]);
        }
        result += "\n";
    }
    return result;
}