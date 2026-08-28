#include "nexus/task/Delay.h"

USING_NAMESPACE_NXS;

Delay::Delay(double delay, Ref<ITimeSource> timeSource)
    : m_timeout(false)
{
    m_timer = std::make_unique<Timer>(timeSource);
    m_timer->Start();
    m_timer->ScheduleAction([this]() {
        m_timeout = true;
    }, delay);
}

bool Delay::Update()
{
    m_timer->Tick();
    return !m_timeout;
}