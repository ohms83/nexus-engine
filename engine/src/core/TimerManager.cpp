//
// Created by nutta on 7/30/2025.
//

#include "nexus/core/TimerManager.h"
#include "nexus/core/Logger.h"

#include <algorithm>

USING_NAMESPACE_NXS;

static Ptr<TimerManager> s_instance;

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
    NXS_ASSERT_MSG(s_instance, "TimerManager instance is not initialized");
    return *s_instance;
}

Ref<ITimer> TimerManager::GetTimer()
{
    Ref<ITimer> timer = std::make_shared<Timer>();
    timer->Tick();
    m_timers.push_back(timer);
    return timer;
}

void TimerManager::RemoveTimer(const Ref<ITimer>& timer)
{
    std::erase(m_timers, timer);
}

void TimerManager::CountDown(const float seconds, const TimerCallback& callback)
{
    const Ref<ITimer> timer = std::make_shared<Timer>();
    timer->CountDown(seconds, callback);
    m_countDownTimers.push_back(timer);
}

void TimerManager::Tick()
{
    std::ranges::for_each(m_timers, [&](const Ref<ITimer>& timer)
    {
        timer->Tick();
    });

    std::vector<Ref<ITimer>> toBeRemoved;
    std::ranges::for_each(m_countDownTimers, [&](const Ref<ITimer>& timer)
    {
        if (timer->Tick()) toBeRemoved.push_back(timer);
    });

    for (const auto& timer : toBeRemoved)
    {
        std::erase(m_countDownTimers, timer);
    }
}
