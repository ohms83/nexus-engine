//
// Created by nutta on 9/5/2025.
//

#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/time/Timer.h"

#include "Runnable.h"

NXS_NAMESPACE
{
    using TaskID = std::uint32_t;

    class TaskScheduler
    {
    public:
        enum class TaskQueue
        {
            MainThread,
            WorkerThread,
        };

        explicit TaskScheduler(const Ref<ITimeSource>& timeSource);
        ~TaskScheduler() = default;

        /**
         * Schedule a task.
         * @param task The task to run.
         * @param repeat How many times this task needs to be repeated.
         * 0 and 1 mean 1-shot, while -1 means it will be repeated indefinitely
         * until it stops or is terminated.
         * @param delay Delay time in seconds.
         * @param interval Update interval. 0 means the task will be updated every frame.
         * @param queue Specific task queue to run this task.
         * @return Task ID.
         */
        MAYBE_UNUSED TaskID ScheduleTask(Ref<IRunnable> task, int32_t repeat = -1, double delay = 0, double interval = 0, TaskQueue queue = TaskQueue::MainThread);
        void StopTask(TaskID id);

        void Update();

    protected:
        struct TaskData
        {
            TaskID id = 0;
            bool running = false;
            int32_t repeat = 0;
            double delay = 0.0;
            double interval = 0.0;
            double nextUpdate = 0.0;
            Ref<IRunnable> action;
        };
        std::vector<TaskData> m_tasks;

        Ptr<Timer> m_timer;
    };
}