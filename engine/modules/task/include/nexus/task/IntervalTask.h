#pragma once

#include "Runnable.h"
#include "nexus/time/StandardTimeSource.h"
#include "nexus/time/TimeSource.h"

NXS_NAMESPACE
{
    /**
     * @class IntervalTask
     * @brief A task that executes a function at regular time intervals.
     *
     * This task uses an internal timer to ensure its wrapped function is executed
     * only after a specified time interval has elapsed. It is useful for implementing
     * game loops, physics updates, or other periodic events.
     */
    class IntervalTask final : public IRunnable
    {
    public:
        /**
         * @brief Constructs a new IntervalTask object.
         *
         * @param interval The time interval (in seconds) between each task execution.
         * Zero or negative values will cause the task to execute on every update.
         * @param task The callable function to be executed.
         * @param timeSource An optional time source for the timer. Defaults to a
         * standard system time source if not provided.
         */
        IntervalTask(double interval, TaskFunc task, Ref<ITimeSource> timeSource = std::make_shared<StandardTimeSource>());

        /**
         * @brief Destructor.
         */
        ~IntervalTask() override = default;

        /**
         * @brief Performs one step of the task.
         *
         * This method checks the elapsed time since the last invocation. If the
         * specified interval has passed, it executes the wrapped task. The task will
         * continue to run as long as the wrapped function returns `true`.
         *
         * @return @c true if the task is still active and should continue;
         * @c false if the task function returns `false`, signaling it is complete.
         */
        MAYBE_UNUSED bool Update() override;

        /// @brief Checks whether the task is currently active.
        NODISCARD bool IsActive() const { return m_isActive; }

    private:
        /// @brief Time source used to measure interval timing.
        Ref<ITimeSource> m_timeSource;
        /// @brief The callable function to be executed.
        TaskFunc m_task;
        /// @brief The time interval in seconds.
        double m_interval = 0;
        /// @brief The last time the task was executed.
        double m_lastTick = 0.0;
        /// @brief Indicates whether the task is currently active.
        bool m_isActive = true;
    };
}