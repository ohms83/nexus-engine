//
// Created by nutta on 7/8/2025.
//

#include <nexus/core/Timer.h>

USING_NAMESPACE_NXS;

void Timer::Stamp()
{
    m_tick = SDL_GetTicks();
}

float Timer::Seconds() const
{
    return FLOAT_CAST(SDL_GetTicks()) / 1000.0f;
}

float Timer::GetDeltaTime() const
{
    const auto currentTick = SDL_GetTicks();
    return CAST<float>(currentTick - m_tick) / 1000.f;
}
