//
// Created by nutta on 7/30/2025.
//

#pragma once

#include <functional>
#include <nexus/NxsDefine.h>

#include "Timer.h"

NXS_NAMESPACE
{
    /**
     * A class that automatically manage the timer objects. It internally calls @c Timer::Tick
     * every frame and automatically create and dispose the one-shot coundown timers.
     */
    class TimerManager
    {
    public:
        static void Init();
        static void Destroy();

        static TimerManager& Instance();

        //! Get a new timer object.
        Ref<ITimer> GetTimer();
        //! Remove the given timer.
        void RemoveTimer(const Ref<ITimer>& timer);

        //! Begin countdown.
        void CountDown(float seconds, const TimerCallback& callback);

        void Tick();

    private:
        std::vector<Ref<ITimer>> m_timers;
        std::vector<Ref<ITimer>> m_countDownTimers;
    };
}
