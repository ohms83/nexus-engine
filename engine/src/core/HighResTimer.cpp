//
// Created by nutta on 7/8/2025.
//

#include <nexus/core/HighResTimer.h>

USING_NAMESPACE_NXS;

void HighResTimer::Stamp()
{
    m_tick = SDL_GetPerformanceCounter();
}

float HighResTimer::GetDeltaTime() const
{
    const auto currentTick = SDL_GetPerformanceCounter();
    return CAST<float>(currentTick - m_tick) / CAST<float>(SDL_GetPerformanceFrequency());
}
