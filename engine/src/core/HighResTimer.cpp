//
// Created by nutta on 7/8/2025.
//

#include <nexus/core/HighResTimer.h>

USING_NAMESPACE_NXS;

float HighResTimer::GetDeltaTime() const
{
    return CAST<float>(GetTick() - GetPrevTick()) / CAST<float>(SDL_GetPerformanceFrequency());
}

uint64 HighResTimer::Tick_Internal()
{
    return SDL_GetPerformanceCounter();
}
