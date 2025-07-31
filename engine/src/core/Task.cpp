//
// Created by nutta on 7/30/2025.
//
#include "nexus/core/Task.h"
#include "nexus/time/StandardTimeSource.h"

USING_NAMESPACE_NXS;

Task::Task(const Action& action, const int32 repeatCount, const float delay, const float interval, bool runImmediate)
    : m_timer(std::make_shared<StandardTimeSource>())
    , m_action(action)
    , m_repeatCount(repeatCount)
    , m_delay(delay)
    , m_interval(interval)
{
    m_numRepeats = m_repeatCount;
    if (runImmediate) Run();
}

Task::Task(const Action &action, int32 repeatCount, float delay, float interval, bool runImmediate, Ref<ITimeSource> timeSource)
    : m_timer(timeSource)
    , m_action(action)
    , m_repeatCount(repeatCount)
    , m_delay(delay)
    , m_interval(interval)
{
    m_numRepeats = m_repeatCount;
    if (runImmediate) Run();
}

void Task::Run()
{
    if (m_isRunning) return;
    m_isRunning = true;

    m_timer.Start();
    m_timer.ScheduleAction([&]
    {
        Trigger();
    }, m_delay);
}

void Task::Stop()
{
    m_timer.Stop();
    m_isRunning = false;
    m_action = Action();
}

void Task::Update()
{
    if (!m_isRunning) return;
    m_timer.Tick();
}

void Task::Trigger()
{
    if (!m_isRunning) return;

    m_action();

    if (m_repeatCount > 0)
    {
        --m_numRepeats;
        if (m_numRepeats == 0)
        {
            m_isRunning = false;
        }
    }

    m_timer.ScheduleAction([&]
    {
        Trigger();
    }, m_interval);
}
