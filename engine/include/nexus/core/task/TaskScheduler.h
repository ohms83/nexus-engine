//
// Created by nutta on 9/5/2025.
//

#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/time/Timer.h"

#include "Runnable.h"
#include "TaskGroup.h"

#include <vector>
#include <map>

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

        enum class UpdatePhase
        {
            PreUpdate,
            Update,
            PostUpdate
        };

        explicit TaskScheduler(const Ref<ITimeSource>& timeSource);
        ~TaskScheduler() = default;

        /**
         * Schedule a task.
         * @param task The task to run.
         * @param phase
         * @param queue Specific task queue to run this task.
         * @return Task ID.
         */
        MAYBE_UNUSED TaskID ScheduleTask(Ref<IRunnable> task, UpdatePhase phase = UpdatePhase::Update, TaskQueue queue = TaskQueue::MainThread);
        void StopTask(TaskID id);

        void PreUpdate();
        void Update();
        void PostUpdate();

    protected:
        struct TaskData
        {
            TaskID id = 0;
            Ref<IRunnable> action;
        };

        std::vector<TaskData> m_tasks;
        std::map<UpdatePhase, TaskGroup> m_taskGroups;
        std::mutex m_mutex;

    private:
        void UpdateTaskGroup(UpdatePhase phase);
        void OnTaskTerminated(Ref<IRunnable> task);
    };
}