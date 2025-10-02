#pragma once

#include "Runnable.h"
#include <vector>

NXS_NAMESPACE
{
    /**
     * @class TaskGroup
     * @brief A task that manages and updates a group of other tasks in parallel.
     *
     * `TaskGroup` acts as a container for multiple `IRunnable` tasks, updating all
     * of them on each call to its `Update()` method. It is useful for managing a
     * collection of independent tasks that need to run concurrently within a single
     * scheduler loop. The `TaskGroup` itself is considered finished when all of
     * its contained tasks have completed.
     */
    class TaskGroup final : public IRunnable
    {
    public:
        /**
         * @brief Constructs a new, empty TaskGroup.
         */
        TaskGroup() = default;

        /**
         * @brief Virtual destructor to ensure proper cleanup.
         */
        virtual ~TaskGroup() = default;

        /**
         * @brief Adds a task to the group.
         * @param task A `Ref` to the `IRunnable` task to be added.
         */
        void Add(Ref<IRunnable> task)
        {
            m_tasks.push_back(task);
        }

        /**
         * @brief Removes a task from the group.
         * @param task A `Ref` to the `IRunnable` task to be removed.
         */
        void Remove(Ref<IRunnable> task);

        /**
         * @brief Updates all tasks within the group.
         *
         * This method iterates through all tasks in the group, calling `Update()` on each.
         * If a task returns `false` (indicating it has finished), it is removed from the group.
         * The `taskFinishedCallback` signal is emitted for each completed task.
         *
         * @return `false` if the group is empty and all tasks have completed; `true` otherwise.
         */
        MAYBE_UNUSED bool Update() override;

        /**
         * @brief Gets a list of tasks managed by this group.
         */
        const TaskList& GetTasks() const { return m_tasks; }

        size_t GetNumTask() const { return m_tasks.size(); }

        /**
         * @brief A signal emitted when a task within the group has finished.
         */
        TaskCallback taskFinishedCallback;

    private:
        /// @brief The collection of tasks managed by this group.
        TaskList m_tasks;
    };
}