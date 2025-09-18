#include "core/task/IntervalTask.h"

USING_NAMESPACE_NXS;

IntervalTask::IntervalTask(double interval, TaskFunc task, Ref<ITimeSource> timeSource)
    : m_task(task)
    , m_interval(interval)
    , m_deferred(false)
{
    m_timer = std::make_unique<Timer>(timeSource);
    m_timer->Start();
}

bool IntervalTask::Update()
{
    m_timer->Tick();
    if (m_deferred) return true;

    m_timer->ScheduleAction([this]() { m_deferred = false; }, m_interval);
    m_deferred = true;
    return m_task();
}