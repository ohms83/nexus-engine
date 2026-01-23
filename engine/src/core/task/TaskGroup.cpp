#include "core/task/TaskGroup.h"

#include <ranges>

USING_NAMESPACE_NXS;

void TaskGroup::Remove(Ref<IRunnable> task)
{
    auto itr = std::ranges::find(m_tasks, task);
    if (itr == m_tasks.end()) return;

    m_tasks.erase(itr);
}

void TaskGroup::RemoveIf(std::function<bool(Ref<IRunnable>)> predicate)
{
    auto itr = std::ranges::find_if(m_tasks, predicate);
    if (itr == m_tasks.end()) return;

    m_tasks.erase(itr);
}

Ref<IRunnable> TaskGroup::Find(Ref<IRunnable> task)
{
    auto itr = std::ranges::find(m_tasks, task);
    if (itr == m_tasks.end()) return nullptr;
    return *itr;
}

Ref<IRunnable> TaskGroup::FindIf(std::function<bool(Ref<IRunnable>)> predicate)
{
    auto itr = std::ranges::find_if(m_tasks, predicate);
    if (itr == m_tasks.end()) return nullptr;
    return *itr;
}

void TaskGroup::Merge(TaskGroup& other)
{
    m_tasks.insert(m_tasks.end(), other.m_tasks.begin(), other.m_tasks.end());
    other.m_tasks.clear();
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