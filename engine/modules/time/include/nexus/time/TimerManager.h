//
// Created by nutta on 7/30/2025.
//

#pragma once

#include "nexus/NxsDefine.h"

#include "StandardTimeSource.h"
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
        TimerManager();

        static void Init();
        static void Destroy();
        static TimerManager& Instance();

        //! Get a new timer object.
        Ref<Timer> CreateTimer(Ref<ITimeSource> timeSource = std::make_shared<StandardTimeSource>());
        //! Remove the given timer.
        void RemoveTimer(const Ref<Timer>& timer);

        /**
         * Schedule the given @c action to be executed after the delayed time.
         * @param action Action to be executed.
         * @param delay Delayed time in seconds.
         * @param timeSource
         */
        void ScheduleAction(
            const Timer::Action& action,
            float delay,
            Ref<ITimeSource> timeSource = std::make_shared<StandardTimeSource>());

        void Tick();

        Ref<const Timer> GetGlobalTimer() const { return m_globalTimer; }
        //! Get elapsed time in seconds since the application started.
        double GetElapsedTime() const { return m_globalTimer->GetElapsedTime(); }
        //! Get elapsed time in seconds since the previous frame.
        double GetDeltaTime() const { return m_globalTimer->GetDeltaTime(); }

    private:
        std::vector<Ref<Timer>> m_timers;
        //! A list of one-shot timers.
        std::vector<Ref<Timer>> m_oneShotTimers;
        Ref<Timer> m_globalTimer;
    };
}
