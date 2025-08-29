//
// Created by nutta on 7/30/2025.
//

#pragma once

#include "Task.h"

NXS_NAMESPACE
{
    class TaskManager
    {
    public:
        static void Destroy();
        static void Init();
        static TaskManager& Instance();

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
        MAYBE_UNUSED TaskHandle CreateTask(
            const Action& action,
            int32 repeatCount = 0,
            float delay = 0,
            float interval = 0,
            bool runImmediate = true);
        //! Stop the given task. If the @c taskHandle is invalid, nothing will happen.
        void StopTask(TaskHandle taskHandle);

        //! Update all tasks.
        void Update();

    private:
        struct TaskInfo
        {
            TaskHandle handle;
            Ptr<Task> task;
        };
        std::vector<TaskInfo> m_tasks;
    };
}
