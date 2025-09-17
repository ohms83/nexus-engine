#include "core/task/DelayTask.h"

USING_NAMESPACE_NXS;

DelayTask::DelayTask(double delay, Ref<ITimeSource> timeSource)
    : m_timeout(false)
{
    m_timer = std::make_unique<Timer>(timeSource);
    m_timer->Start();
    m_timer->ScheduleAction([this]() {
        m_timeout = true;
    }, delay);
}

bool DelayTask::Update()
{
    m_timer->Tick();
    return !m_timeout;
}