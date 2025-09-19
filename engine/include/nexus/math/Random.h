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
            std::uniform_real_distribution<double> distribution(a, b);
            return distribution(m_rng);
        }
        
        template<typename T>
        requires std::is_floating_point_v<T>
        T Range01()
        {
            std::uniform_real_distribution<double> distribution(0, 1);
            return distribution(m_rng);
        }
    private:
        std::mt19937 m_rng;
    };
}