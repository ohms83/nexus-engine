#pragma once

#include "Runnable.h"
#include "nexus/time/StandardTimeSource.h"
#include "nexus/time/Timer.h"

NXS_NAMESPACE
{
    /**
     * @class Delay
     * @brief A task that simply waits for a specified duration before finishing.
     *
     * This task is a simple, non-blocking delay. It does not perform any action but
     * serves to pause a sequence of tasks or introduce a wait period in a scheduler.
     * The task remains active (Update() returns `true`) until the specified time
     * has elapsed.
     */
    class Delay final : public IRunnable
    {
    public:
        /**
         * @brief Constructs a new Delay task.
         *
         * @param seconds The duration of the delay in seconds.
         * @param timeSource An optional time source for the internal timer.
         * Defaults to a standard system time source.
         */
        explicit Delay(double seconds, Ref<ITimeSource> timeSource = std::make_shared<StandardTimeSource>());

        /**
         * @brief Destructor.
         */
        virtual ~Delay() = default;

        /**
         * @brief Updates the timer and checks if the delay period has expired.
         *
         * This method ticks the internal timer and returns `true` until the scheduled
         * delay has elapsed, at which point it returns `false`.
         *
         * @return @c true if the delay is still in progress; @c false if the delay
         * has completed.
         */
        MAYBE_UNUSED bool Update() override;

    private:
        /// @brief The timer used to track the delay duration.
        Ptr<Timer> m_timer;
        /// @brief A flag to indicate whether the delay has timed out.
        bool m_timeout = false;
    };
}