//
// Created by nutta on 9/5/2025.
//

#pragma once

#include "nexus/NxsDefine.h"
#include "Runnable.h"

#include <future>

NXS_NAMESPACE
{
    template<class ResultType>
    class FutureWaitingTask final : public IRunnable
    {
    public:
        using ResultCallback = std::function<void(ResultType)>;
        using ErrorCallback = std::function<void(const std::string&)>;
        /**
         *
         * @param future
         * @param callback
         * @param error
         */
        explicit FutureWaitingTask(std::future<ResultType>&& future, ResultCallback callback, ErrorCallback error = [](const std::string&){})
            : m_future(std::move(future))
            , m_callback(callback)
            , m_error(error)
        {}

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
        std::future<ResultType> m_future;
        ResultCallback m_callback;
        ErrorCallback m_error;
    };
}
