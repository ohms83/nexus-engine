#include "nexus/task/IntervalTask.h"

USING_NAMESPACE_NXS;

IntervalTask::IntervalTask(double interval, TaskFunc task, Ref<ITimeSource> timeSource)
    : m_timeSource(timeSource)
    , m_task(task)
    , m_interval(interval)
    , m_lastTick(timeSource ? timeSource->Now() : 0.0)
{
}

bool IntervalTask::Update()
{
    if (!m_isActive || !m_task || !m_timeSource) return false;

    const double now = m_timeSource ? m_timeSource->Now() : 0.0;
    if (now - m_lastTick < m_interval)
    {
        return true;
    }

    m_lastTick = now;
    m_isActive = m_task();
    return m_isActive;
}