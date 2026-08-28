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

        TaskGroup(TaskGroup&& other)
        {
            m_tasks = std::move(other.m_tasks);
            taskFinishedCallback = std::move(other.taskFinishedCallback);
        }

        TaskGroup& operator=(TaskGroup&& other)
        {
            m_tasks = std::move(other.m_tasks);
            taskFinishedCallback = std::move(other.taskFinishedCallback);
            return *this;
        }


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
         * @brief Removes the first task that satisfies the given predicate.
         * This method searches the task list using a linear search. If a match is found,
         * it is removed from the group, potentially triggering the destruction of the 
         * task if no other references exist.
         * @param predicate A callable that takes a @c Ref<IRunnable> and returns true for the target task.
         */
        void RemoveIf(std::function<bool(Ref<IRunnable>)> predicate);

        /**
         * @brief Finds a specific task within the group.
         * @param task The reference to the task to search for.
         * @return The @c Ref<IRunnable> if found; otherwise, @c nullptr.
         */
        Ref<IRunnable> Find(Ref<IRunnable> task);
        /**
         * @brief Finds the first task that satisfies the given predicate.
         * @param predicate A callable that returns true for the desired task.
         * @return The first matching @c Ref<IRunnable> found; otherwise, @c nullptr.
         */
        Ref<IRunnable> FindIf(std::function<bool(Ref<IRunnable>)> predicate);

        /**
         * @brief Merges another TaskGroup into this one.
         * All tasks from the @p other group are appended to the end of this group's 
         * task list. Following the merge, the @p other group is cleared of all tasks.
         * @note This operation is efficient as it moves the internal references without 
         * re-allocating the underlying @c IRunnable objects.
         * @param other The TaskGroup to be merged and cleared.
         */
        void Merge(TaskGroup& other);

        /**
         * @brief Clears all tasks from the group.
         */
        void Clear()
        {
            m_tasks.clear();
        }

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