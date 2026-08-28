#pragma once

#include "Runnable.h"
#include "nexus/time/StandardTimeSource.h"
#include "nexus/time/Timer.h"

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
         * This method checks the internal timer. If the specified interval has
         * passed, it executes the wrapped task. The task will continue to
         * run as long as the wrapped function returns `true`.
         *
         * @return @c true if the task is still active and should continue;
         * @c false if the task function returns `false`, signaling it is complete.
         */
        MAYBE_UNUSED bool Update() override;

    private:
        /// @brief The internal timer used to track the interval.
        Ptr<Timer> m_timer;
        /// @brief The callable function to be executed.
        TaskFunc m_task;
        /// @brief The time interval in seconds.
        double m_interval = 0;
        /**
         * @brief A flag used to synchronize with the timer's scheduled action.
         * The task will not execute the wrapped function until the scheduled action
         * has been called by the timer.
         */
        bool m_deferred = false;
    };
}