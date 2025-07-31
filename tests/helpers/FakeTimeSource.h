#pragma once

#include "nexus/Nexus.h"

class FakeTimeSource final : public nxs::ITimeSource
{
public:
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