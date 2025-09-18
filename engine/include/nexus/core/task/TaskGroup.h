#pragma once

#include "Runnable.h"
#include <vector>

NXS_NAMESPACE
{
    class TaskGroup final : public IRunnable
    {
    public:
        TaskGroup() = default;
        virtual ~TaskGroup() = default;

        void Add(Ref<IRunnable> task)
        {
            m_tasks.push_back(task);
        }
        void Remove(Ref<IRunnable> task);

        MAYBE_UNUSED bool Update() override;

        TaskCallback taskFinishedCallback;
    private:
        std::vector<Ref<IRunnable>> m_tasks;
    };
}