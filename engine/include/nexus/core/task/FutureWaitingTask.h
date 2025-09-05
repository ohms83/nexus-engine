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
        explicit FutureWaitingTask(std::future<ResultType>&& future, ResultCallback callback)
            : m_future(std::move(future))
            , m_callback(callback)
        {}

        bool Update() override
        {
            if (!m_future.valid() || m_future.wait_for(std::chrono::seconds(0)) != std::future_status::ready) return true;
            m_callback(m_future.get());
            return false;
        }
    private:
        std::future<ResultType> m_future;
        ResultCallback m_callback;
    };
}
