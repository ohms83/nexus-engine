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
    /**
     * @typedef TaskID
     * @brief A unique identifier for a scheduled task.
     */
    using TaskID = std::uint32_t;

    /**
     * @class TaskScheduler
     * @brief A central manager for scheduling and executing tasks.
     *
     * The TaskScheduler provides a flexible system for managing and running tasks
     * within a main game loop. Tasks can be scheduled to run on the main thread or
     * worker threads, and can be organized into different update phases (pre-update,
     * update, post-update) to ensure they run in the correct order.
     *
     * It uses a combination of `TaskGroup` objects and a mutex to provide thread-safe
     * scheduling and execution management.
     */
    class TaskScheduler
    {
    public:
        /**
         * @enum TaskQueue
         * @brief Specifies the execution context for a task.
         */
        enum class TaskQueue
        {
            MainThread,   ///< The task is scheduled to run on the main thread.
            WorkerThread, ///< The task is scheduled to run on a worker thread.
        };

        /**
         * @enum UpdatePhase
         * @brief Defines the update phase in which a task will be executed.
         */
        enum class UpdatePhase
        {
            PreUpdate,  ///< The task is executed at the beginning of the update loop.
            Update,     ///< The task is executed during the main update phase.
            PostUpdate  ///< The task is executed at the end of the update loop.
        };

        /**
         * @brief Constructs a new TaskScheduler object.
         * @param timeSource A reference to the time source used by tasks for timing.
         */
        explicit TaskScheduler(const Ref<ITimeSource>& timeSource);

        /**
         * @brief Destructor.
         */
        ~TaskScheduler() = default;

        /**
         * @brief Schedules a new task to be managed by the scheduler.
         *
         * @param task The task to be scheduled.
         * @param phase The update phase in which the task should run.
         * @param queue The specific queue (MainThread or WorkerThread) for task execution.
         * @return The unique ID assigned to the scheduled task.
         * @note This function is thread-safe.
         */
        MAYBE_UNUSED TaskID ScheduleTask(Ref<IRunnable> task, UpdatePhase phase = UpdatePhase::Update, TaskQueue queue = TaskQueue::MainThread);

        /**
         * @brief Stops and removes a task by its ID.
         *
         * This method removes the task from the scheduler, preventing it from being
         * updated in subsequent frames.
         * @param id The unique ID of the task to stop.
         * @note This function is thread-safe.
         */
        void StopTask(TaskID id);

        /**
         * @brief Executes all tasks scheduled for the PreUpdate phase.
         */
        void PreUpdate();

        /**
         * @brief Executes all tasks scheduled for the main Update phase.
         */
        void Update();

        /**
         * @brief Executes all tasks scheduled for the PostUpdate phase.
         */
        void PostUpdate();

    protected:
        /**
         * @brief A struct to store task information.
         */
        struct TaskData
        {
            TaskID id = 0;           ///< The unique task ID.
            Ref<IRunnable> action;   ///< A reference to the IRunnable task.
        };

        std::vector<TaskData> m_tasks;
        std::map<UpdatePhase, TaskGroup> m_taskGroups;
        std::mutex m_mutex;

    private:
        /**
         * @brief Internal helper to update a specific task group.
         * @param phase The update phase of the group to update.
         */
        void UpdateTaskGroup(UpdatePhase phase);

        /**
         * @brief Callback function for when a task in a TaskGroup terminates.
         * @param task A reference to the terminated task.
         */
        void OnTaskTerminated(Ref<IRunnable> task);
    };
}