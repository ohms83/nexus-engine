#include "core/task/TaskGroup.h"

#include <ranges>

USING_NAMESPACE_NXS;

void TaskGroup::Remove(Ref<IRunnable> task)
{
    auto itr = std::ranges::find(m_tasks, task);
    if (itr == m_tasks.end()) return;

    m_tasks.erase(itr);
}

bool TaskGroup::Update()
{
    std::vector<Ref<IRunnable>> terminatedTasks;
    for (auto task : m_tasks)
    {
        if (!task->Update())
        {
            terminatedTasks.push_back(task);
            taskFinishedCallback(task);
        }
    }

    while (!terminatedTasks.empty())
    {
        auto top = terminatedTasks.begin();
        Remove(*top);
        terminatedTasks.erase(top);
    }

    return !m_tasks.empty();
}