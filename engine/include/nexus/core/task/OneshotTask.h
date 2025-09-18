#pragma once

#include "Runnable.h"
#include "../Function.h"

NXS_NAMESPACE
{
    class OneshotTask final : public IRunnable
    {
    public:
        OneshotTask(Action task) : m_task(task) {}
        ~OneshotTask() override = default;

        MAYBE_UNUSED bool Update()
        {
            m_task();
            return false;
        }

    private:
        Action m_task;
    };
}