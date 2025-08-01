//
// Created by nutta on 7/30/2025.
//

#pragma once

#include "nexus/NxsDefine.h"
#include "../time/Timer.h"

NXS_NAMESPACE
{
    using TaskHandle = uint32;

    class Task
    {
    public:
        /**
         * Create a task.
         * @param action A task to run.
         * @param repeatCount How much time this task should be repeated. 0 or 1 means it's a one-shot while
         * a minus value means the task will run indefinitely.
         * @param delay A delay in seconds before the task to run.
         * @param interval How many seconds the task should wait between each repeat. 0 means the task will run every frame.
         * @param runImmediate If @c true, the task will start running immediately; otherwise, it must be manually started
         * by calling @c Run funciton.
         */        
        Task(const Action& action, int32 repeatCount, float delay, float interval, bool runImmediate = true);
        Task(const Action& action, int32 repeatCount, float delay, float interval, bool runImmediate, Ref<ITimeSource> timeSource);
        virtual ~Task() = default;

        NODISCARD uint32 GetRepeatCount() const
        {
            return m_repeatCount;
        }

        NODISCARD bool IsFinite() const
        {
            return m_repeatCount > 0;
        }

        NODISCARD bool IsRunning() const
        {
            return m_isRunning;
        }

        NODISCARD bool IsFinished() const
        {
            return IsFinite() && m_numRepeats == 0;
        }

        NODISCARD float GetDelay() const
        {
            return m_delay;
        }

        NODISCARD float GetDuration() const
        {
            return m_interval;
        }

        //! Begin the task.
        void Run();
        void Stop();

        //! Get time since the last update.
        NODISCARD float GetDeltaTime() const
        {
            return m_timer.GetDeltaTime();
        }

        virtual void Update();

    protected:
        Timer m_timer;
        Action m_action;
        int32 m_repeatCount = 0;
        int32 m_numRepeats = 0;
        float m_delay = 0;
        float m_interval = 0;
        bool m_isRunning = false;

        //! Trigger the action.
        void Trigger();
    };
}
