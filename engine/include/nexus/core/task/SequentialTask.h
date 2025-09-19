#pragma once

#include "Runnable.h"
#include <queue>

NXS_NAMESPACE
{
    /**
     * @class SequentialTask
     * @brief A task that executes a sequence of other tasks one after another.
     *
     * This task acts as a container for a series of `IRunnable` tasks, which it
     * executes sequentially. When one task in the sequence finishes (i.e., its
     * `Update()` method returns `false`), the `SequentialTask` automatically
     * starts the next task in the queue. The entire sequence is complete when all
     * tasks in the queue have been executed.
     */
    class SequentialTask final : public IRunnable
    {
    public:
        /**
         * @brief Constructs a new, empty SequentialTask.
         */
        SequentialTask() = default;

        /**
         * @brief Constructs a SequentialTask from an initializer list of tasks.
         * @param taskList An initializer list containing the tasks to be added
         * to the sequence.
         */
        SequentialTask(std::initializer_list<Ref<IRunnable>> taskList);

        /**
         * @brief Virtual destructor to ensure proper cleanup.
         */
        virtual ~SequentialTask() = default;

        /**
         * @brief Adds a task to the end of the sequence.
         * @param task A `Ref` to the `IRunnable` task to be added.
         */
        void Push(Ref<IRunnable> task) { m_tasks.push(task); }

        /**
         * @brief Removes and returns the next task from the sequence.
         * @return A `Ref` to the next `IRunnable` task, or `nullptr` if the queue is empty.
         */
        MAYBE_UNUSED Ref<IRunnable> Pop();

        /**
         * @brief Updates the currently running task in the sequence.
         *
         * This method first checks if there is a task currently running. If not, it
         * attempts to pop a new task from the queue. It then calls `Update()` on
         * the current task. If the current task finishes, it is reset to `nullptr`.
         *
         * @return `false` if the entire sequence is complete; `true` otherwise.
         */
        MAYBE_UNUSED bool Update() override;

    private:
        /// @brief The task currently being executed.
        Ref<IRunnable> m_runningTask;
        /// @brief A queue of pending tasks to be executed.
        std::queue<Ref<IRunnable>> m_tasks;
    };
}