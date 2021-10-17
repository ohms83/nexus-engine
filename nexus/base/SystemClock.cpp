//
//  SystemClock.cpp
//  Lapis
//
//  Created by nuttachai on 21/5/20.
//

#include "SystemClock.hpp"

USING_NAMESPACE_NXS;
using namespace std::chrono;

void SystemClock::tick()
{
    m_marker = system_clock::now();
}

uint32_t SystemClock::getDiffMilliseconds() const
{
    return (uint32_t)getDiff<milliseconds>();
}

uint32_t SystemClock::getDiffSeconds() const
{
    return (uint32_t)getDiff<seconds>();
}

uint64_t SystemClock::getDiffMicroseconds() const
{
    return getDiff<microseconds>();
}
