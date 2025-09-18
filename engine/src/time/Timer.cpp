//
// Created by nutta on 7/8/2025.
//

#include "time/Timer.h"
#include "math/Math.h"

USING_NAMESPACE_NXS;

Timer::Timer(const Ref<ITimeSource>& timeSource)
    : m_timeSource(timeSource)
{
}

void Timer::Start()
{
    m_startTime = m_currentTick = m_lastTick = m_timeSource->Now();
    m_isRunning = true;
}

void Timer::Stop()
{
    m_startTime = m_currentTick = m_lastTick = 0;
    m_isRunning = false;
    m_isExecuted = false;
    m_scheduledAction = nullptr;
}

void Timer::Tick()
{
    if (!m_isRunning) return;

    m_lastTick = m_currentTick;
    m_currentTick = m_timeSource->Now();

    if (m_scheduledAction && !m_isExecuted)
    {
        m_countDown -= GetDeltaTime();
        if (Math::Compare(m_countDown, 0, m_faultTolerance) <= 0)
        {
            m_isExecuted = true;
            m_countDown = 0;
            m_scheduledAction();
        }
    }
}

float Timer::GetDeltaTime() const
{
    return FLOAT_CAST(m_currentTick - m_lastTick);
}

float Timer::GetElapsedTime() const
{
    if (!m_isRunning) return 0;
    return FLOAT_CAST(m_timeSource->Now() - m_startTime);
}

void Timer::ScheduleAction(const Action& action, const float delay)
{
    m_countDown = delay;
    m_scheduledAction = action;
    m_isExecuted = false;
}
