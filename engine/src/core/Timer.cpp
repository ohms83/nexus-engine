//
// Created by nutta on 7/8/2025.
//

#include <nexus/core/Timer.h>

USING_NAMESPACE_NXS;

bool ITimer::Tick()
{
    bool result = false;
    if (m_countDown > 0)
    {
        m_countDown -= GetDeltaTime();
        if (m_countDown <= 0)
        {
            m_callback();
            m_countDown = 0.0f;
            result = true;
        }
    }

    m_prevTick = m_tick;
    m_tick = Tick_Internal();
    return result;
}

void ITimer::CountDown(const float seconds, const TimerCallback& callback)
{
    m_tick = Tick_Internal();
    m_countDown = seconds;
    m_callback = callback;
}

float Timer::GetDeltaTime() const
{
    return CAST<float>(GetTick() - GetPrevTick()) / 1000.f;
}

uint64 Timer::Tick_Internal()
{
    return SDL_GetTicks();
}
