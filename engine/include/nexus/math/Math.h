#pragma once

#include <type_traits>

#include <nexus/NxsDefine.h>
#include <nexus/NxsCommon.h>

#define NXS_INFINITE 0xFFFFFFFF

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
        requires std::is_floating_point_v<T>
        T Lerp(T a, T b, float t)
        {
            return a + (b - a) * t;
        }

        template <typename T>
        requires is_vector<T> ||
            std::derived_from<T, glm::vec2> || std::derived_from<T, glm::vec3> ||
            std::derived_from<T, glm::vec4>
        T VLerp(const T& a, const T& b, float t)
        {
            return T (a + (b - a) * t);
        }
    }
}