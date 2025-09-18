#pragma once

#include "Runnable.h"

NXS_NAMESPACE
{
    class RepeatTask final : public IRunnable
    {
    public:
        explicit RepeatTask(int32_t numRepeat, TaskFunc task)
            : m_numRepeat(numRepeat)
            , m_task(task)
        {}
        virtual ~RepeatTask() = default;
        MAYBE_UNUSED bool Update() override;

    private:
        int32_t m_numRepeat = 0;
        TaskFunc m_task;
    };
}