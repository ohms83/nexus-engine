#pragma once

#include "Runnable.h"

NXS_NAMESPACE
{
    /**
     * @class RepeatTask
     * @brief A task that executes a given function a specified number of times.
     *
     * This class wraps a `TaskFunc` callable and repeatedly executes it on each
     * call to its Update() method. The task can be set to run for a finite number
     * of repetitions or indefinitely.
     *
     * The task is considered finished when the number of repetitions is exhausted
     * or when the wrapped `TaskFunc` returns `false`.
     */
    class RepeatTask final : public IRunnable
    {
    public:
        /**
         * @brief Constructs a new RepeatTask object.
         *
         * @param numRepeat The number of times the task should be repeated.
         * - If `numRepeat` is a positive value, the task will run
         * for that many updates.
         * - If `numRepeat` is `0`, the task will terminate immediately
         * without running.
         * - If `numRepeat` is a negative value, the task will be
         * repeated indefinitely until the provided `task` returns `false`.
         * @param task A `TaskFunc` callable to be executed on each update.
         * @pre `task` must be a valid, non-empty `std::function`.
         */
        explicit RepeatTask(int32_t numRepeat, TaskFunc task)
            : m_numRepeat(numRepeat)
            , m_task(task)
        {}

        /**
         * @brief Destructor.
         */
        virtual ~RepeatTask() = default;

        /**
         * @brief Performs one step of the task.
         *
         * This method executes the wrapped task and decrements the internal
         * repetition counter.
         *
         * @return @c false if the task has finished its repetitions or if the wrapped
         * `TaskFunc` returns `false`. Returns @c true otherwise.
         */
        MAYBE_UNUSED bool Update() override;

    private:
        /// @brief The number of times the task will repeat.
        int32_t m_numRepeat = 0;
        /// @brief The callable function to be executed.
        TaskFunc m_task;
    };
}