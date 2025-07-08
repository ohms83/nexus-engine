//
// Created by nutta on 7/8/2025.
//

#include <nexus/core/SystemClock.h>

USING_NAMESPACE_NXS;
using namespace std::chrono;

void SystemClock::Tick()
{
    m_marker = system_clock::now();
}

uint32 SystemClock::GetDiffSeconds() const
{
    return CAST<uint32>(GetDiff<seconds>());
}

uint32 SystemClock::GetDiffMilliseconds() const
{
    return CAST<uint32>(GetDiff<milliseconds>());
}

uint64_t SystemClock::GetDiffMicroseconds() const
{
    return GetDiff<microseconds>();
}
