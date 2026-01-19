#pragma once

#include "Runnable.h"
#include "../Function.h"

NXS_NAMESPACE
{
    /**
     * @class OneshotTask
     * @brief A task that executes a given function once and then finishes.
     *
     * This class wraps a simple, one-time callable object. When its Update() method
     * is called, it executes the wrapped function and immediately returns `false`,
     * signaling to the scheduler that the task is complete and should be removed.
     */
    class OneshotTask final : public IRunnable
    {
    public:
        /**
         * @brief Constructs a new OneshotTask object.
         * @param task A callable object to be executed once.
         */
        OneshotTask(Action task) : m_task(task) {}

        /**
         * @brief Destructor.
         */
        ~OneshotTask() override = default;

        /**
         * @brief Executes the wrapped task once.
         *
         * This method calls the internal action and then returns `false` to
         * indicate that the task is complete.
         *
         * @return Always returns `false`.
         */
        MAYBE_UNUSED bool Update() override
        {
            m_task();
            return false;
        }

    private:
        /// @brief The callable object to be executed.
        Action m_task;
    };
}