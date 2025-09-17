#pragma once

#include "Runnable.h"

NXS_NAMESPACE
{
    class RepeatTask final : public IRunnable
    {
    public:
        using Func = std::function<bool()>;

        explicit RepeatTask(uint32_t numRepeat, Func task);
        virtual ~RepeatTask() = default;
        bool Update() override;

    private:
        uint32_t m_numRepeat = 0;
        Func m_task;
    };
}