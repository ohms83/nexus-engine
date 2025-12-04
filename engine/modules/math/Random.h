#pragma once

#include "nexus/NxsCommon.h"
#include <random>

NXS_NAMESPACE
{
    class Random
    {
    public:
        Random();

        template<typename T>
        requires std::is_floating_point_v<T>
        T Range(T a, T b)
        {
            std::uniform_real_distribution<T> distribution(a, b);
            return distribution(m_rng);
        }

        template<typename T>
        requires std::is_floating_point_v<T>
        T Range01()
        {
            std::uniform_real_distribution<T> distribution(0, 1);
            return distribution(m_rng);
        }

        template<typename T>
        requires std::is_floating_point_v<T>
        glm::vec3 RangeVec(T a, T b)
        {
            return glm::vec3(Range(a, b), Range(a, b), Range(a, b));
        }
    private:
        std::mt19937 m_rng;
    };
}