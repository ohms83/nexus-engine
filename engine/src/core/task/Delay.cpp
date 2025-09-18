#include "core/task/Delay.h"
#include "core/LogDispatcher.h"

USING_NAMESPACE_NXS;

Delay::Delay(double delay, Ref<ITimeSource> timeSource)
    : m_timeout(false)
{
    m_timer = std::make_unique<Timer>(timeSource);
    m_timer->Start();
    m_timer->ScheduleAction([this]() {
        LOG_DEBUG(LogTemp, std::format("Delay ended at {}", m_timer->GetElapsedTime()));
        m_timeout = true;
    }, delay);
}

bool Delay::Update()
{
    m_timer->Tick();
    return !m_timeout;
}