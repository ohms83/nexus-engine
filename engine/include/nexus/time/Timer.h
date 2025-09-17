//
// Created by nutta on 7/8/2025.
//

#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/core/Function.h"
#include "TimeSource.h"
#include "sigslot/signal.hpp"

NXS_NAMESPACE
{
    class Timer final
    {
    public:
        explicit Timer(const Ref<ITimeSource>& timeSource);

        void Start();
        void Stop();
        //! Advance the time counter.
        void Tick();
        //! Get the number of seconds since the last @c Tick.
        NODISCARD float GetDeltaTime() const;
        /**
         * Get the number of seconds since the @c Start function was called.
         * @return The number of seconds since the @c Start function was called or 0 if
         * the timer was stopped or hasn't been started yet.
         */
        NODISCARD float GetElapsedTime() const;

        /**
         * Schedule the given @c action to be executed after the delayed time.
         * @param action Action to be executed.
         * @param delay Delayed time in seconds.
         */
        void ScheduleAction(const Action& action, float delay);

        //! Check whether the scheduled action was already executed.
        NODISCARD bool IsExecuted() const
        {
            return m_isExecuted;
        }

    protected:
        Ref<ITimeSource> m_timeSource;
        double m_startTime = 0;;
        double m_currentTick = 0;
        double m_lastTick = 0;
        bool m_isRunning = false;

        Action m_scheduledAction = nullptr;
        bool m_isExecuted = false;
        float m_countDown = 0;
    };
}
