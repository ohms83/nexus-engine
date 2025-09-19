//
// Created by nutta on 9/5/2025.
//

#pragma once

#include "nexus/NxsDefine.h"
#include "sigslot/signal.hpp"

DECLARE_LOG_EXTERN(Runnable);

NXS_NAMESPACE
{
    /**
     * @class IRunnable
     * @brief An abstract base class for a task that can be executed.
     *
     * This interface defines a contract for objects that can perform work, typically
     * over multiple discrete steps or updates. A task is considered "runnable" as long
     * as its Update method returns true, indicating it needs more processing time.
     *
     * @note The Update method is not guaranteed to be thread-safe unless implemented
     * with synchronization primitives.
     */
    class IRunnable
    {
    public:
        /**
         * @brief Virtual destructor to ensure proper cleanup of derived classes.
         */
        virtual ~IRunnable() = default;

        /**
         * @brief Performs a step of the task's work.
         *
         * This method should contain a small, manageable unit of work for the task.
         * The scheduler will call this method repeatedly until it returns false,
         * indicating that the task is complete.
         *
         * @return @c false if the task has finished and can be destroyed; otherwise, @c true to indicate
         * that the task needs more updates.
         */
        MAYBE_UNUSED virtual bool Update() = 0;
    };

    /**
     * @typedef TaskFunc
     * @brief A standard function object type for tasks.
     *
     * Represents a callable that takes no arguments and returns a boolean,
     * indicating whether the task is complete. This is used for simple, stateless tasks.
     */
    using TaskFunc = std::function<bool()>;

    /**
     * @typedef TaskCallback
     * @brief A signal for task-related events.
     *
     * This is a signal that can be used to notify observers about a task, for example,
     * when it is finished. The signal carries a reference to the IRunnable object.
     */
    using TaskCallback = sigslot::signal<Ref<IRunnable>>;
}