//
// Created by nutta on 7/30/2025.
//

#include "nexus/time/TimerManager.h"
#include "nexus/time/StandardTimeSource.h"

#include <algorithm>

USING_NAMESPACE_NXS;

static Ptr<TimerManager> s_instance;

TimerManager::TimerManager()
{
    m_globalTimer = std::make_shared<Timer>(std::make_shared<StandardTimeSource>());
    m_globalTimer->Start();
}

void TimerManager::Init()
{
    s_instance.reset(new TimerManager());
}

void TimerManager::Destroy()
{
    s_instance.reset();
}

TimerManager& TimerManager::Instance()
{
    // TODO: Decoupling assertion from LogDispatcher to avoid circular dependency
    // NXS_ASSERT_MSG(s_instance, "TimerManager instance is not initialized");
    return *s_instance;
}

Ref<Timer> TimerManager::CreateTimer(Ref<ITimeSource> timeSource)
{
    const auto timer = std::make_shared<Timer>(timeSource);
    timer->Start();
    m_timers.push_back(timer);
    return timer;
}

void TimerManager::RemoveTimer(const Ref<Timer>& timer)
{
    std::erase(m_timers, timer);
}

void TimerManager::ScheduleAction(const Timer::Action& action, const float delay, Ref<ITimeSource> timeSource)
{
    const auto timer = std::make_shared<Timer>(timeSource);
    timer->ScheduleAction(action, delay);
    m_oneShotTimers.push_back(timer);
}

void TimerManager::Tick()
{
    m_globalTimer->Tick();

    std::ranges::for_each(m_timers, [&](const Ref<Timer>& timer)
    {
        timer->Tick();
    });

    std::ranges::for_each(m_oneShotTimers, [&](const Ref<Timer>& timer)
    {
        timer->Tick();
    });
    std::erase_if(m_oneShotTimers, [&](const Ref<Timer>& timer)
    {
        return timer->IsExecuted();
    });
}
