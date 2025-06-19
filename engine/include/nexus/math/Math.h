#pragma once

#include <type_traits>

#include <NxsDefine.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

NXS_NAMESPACE
{
    namespace Math
    {
        template <typename T>
        constexpr bool is_vector = false;
        template <>
        constexpr bool is_vector<glm::vec2> = true;
        template <>
        constexpr bool is_vector<glm::vec3> = true;
        template <>
        constexpr bool is_vector<glm::vec4> = true;

        template <typename T>
        requires std::is_floating_point_v<T> || is_vector<T>
        T Lerp(T a, T b, float t)
        {
            return a + (b - a) * t;
        }
    }
}