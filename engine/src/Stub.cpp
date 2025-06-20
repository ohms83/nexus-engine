// A temporary file using purely for integration testing
#include <format>
#include <iostream>
#include <glm/common.hpp>
#include <math/Math.h>
#include <SDL3/SDL.h>

void Test()
{
    constexpr auto v1 = glm::vec3(1.0f, 2.0f, 3.0f);
    constexpr auto v2 = glm::vec3(2.0f, 4.0f, -10.0f);
    const auto v3 = nexus::Math::VLerp(v1, v2, 0.5f);
    std::cout << "Hello World!\n";
    std::cout << std::format("v3: {:.2f}, {:.2f}, {:.2f}!\n", v3.x, v3.y, v3.z);
}