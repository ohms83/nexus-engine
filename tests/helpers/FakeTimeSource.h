#pragma once

#include "nexus/Nexus.h"
#include <random>
#include <chrono>

class FakeTimeSource final : public nxs::ITimeSource
{
public:
    FakeTimeSource()
    {
        // Randomize the starting value to simulate the real-world use case.
        std::mt19937 rng(std::chrono::high_resolution_clock::now().time_since_epoch().count());
        // Create a uniform real distribution for the desired range.
        std::uniform_real_distribution<double> distribution(0.0, 100.0);
        seconds_ = distribution(rng);
    }

    double Now() override
    {
        return seconds_;
    }
    double NowMs() override
    {
        return seconds_ * 1000;
    }

    //! Manually advance to tick by the specified seconds
    void Advance(const double seconds)
    {
        seconds_ += seconds;
    }

private:
    double seconds_ = 0.0;
};