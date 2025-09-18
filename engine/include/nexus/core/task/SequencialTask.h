#pragma once

#include "Runnable.h"
#include <queue>

NXS_NAMESPACE
{
    class SequencialTask final : public IRunnable
    {
    public:
        SequencialTask() = default;
        virtual ~SequencialTask() = default;

        void Push(Ref<IRunnable> task) { m_tasks.push(task); }

        MAYBE_UNUSED Ref<IRunnable> Pop();

        MAYBE_UNUSED bool Update() override;
    private:
        Ref<IRunnable> m_runningTask;
        std::queue<Ref<IRunnable>> m_tasks;
    };
}