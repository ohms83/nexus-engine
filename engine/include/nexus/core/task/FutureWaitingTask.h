//
// Created by nutta on 9/5/2025.
//

#pragma once

#include "nexus/NxsDefine.h"
#include "Runnable.h"

#include <future>

NXS_NAMESPACE
{
    /**
     * @class FutureWaitingTask
     * @brief A task that waits for a std::future to become ready.
     *
     * @tparam ResultType The type of the value held by the std::future.
     *
     * This task polls a `std::future` to check if its asynchronous operation has completed.
     * When the future is ready, it retrieves the result, invokes a success callback with the result,
     * and then signals that it is complete. If the future contains an exception, it invokes an
     * error callback with the exception message.
     *
     * This pattern allows asynchronous operations to be integrated into a single-threaded
     * task scheduler or game loop without blocking.
     */
    template<class ResultType>
    class FutureWaitingTask final : public IRunnable
    {
    public:
        /**
         * @typedef ResultCallback
         * @brief A callback function type for a successful result.
         *
         * The callback is invoked when the `std::future` becomes ready and successfully
         * returns a value.
         */
        using ResultCallback = std::function<void(ResultType)>;
        
        /**
         * @typedef ErrorCallback
         * @brief A callback function type for an error.
         *
         * The callback is invoked if the `std::future` contains an exception.
         * It receives the exception message as a string.
         */
        using ErrorCallback = std::function<void(const std::string&)>;

        /**
         * @brief Constructs a new FutureWaitingTask object.
         *
         * @param future A `std::future` object that is moved into the task. The task
         * takes ownership of the future.
         * @param callback A `ResultCallback` to be invoked on success.
         * @param error An optional `ErrorCallback` to be invoked on failure. A default
         * empty callback is used if not provided.
         */
        explicit FutureWaitingTask(std::future<ResultType>&& future, ResultCallback callback, ErrorCallback error = [](const std::string&){})
            : m_future(std::move(future))
            , m_callback(callback)
            , m_error(error)
        {}

        /**
         * @brief Checks the status of the future and handles the result.
         *
         * This method checks the `std::future`'s status without blocking. If the future is
         * ready, it gets the result and calls the appropriate callback.
         *
         * @return @c true if the task is still waiting for the future to be ready;
         * @c false if the task has completed and can be removed.
         */
        MAYBE_UNUSED bool Update() override
        {
            if (!m_future.valid() || m_future.wait_for(std::chrono::seconds(0)) != std::future_status::ready) return true;
            try {
                m_callback(m_future.get());
            }
            catch (const std::exception& e) {
                m_error(e.what());
            }
            return false;
        }

    private:
        /// @brief The future object this task is waiting on.
        std::future<ResultType> m_future;
        /// @brief The callback for a successful result.
        ResultCallback m_callback;
        /// @brief The callback for handling an error.
        ErrorCallback m_error;
    };
}
