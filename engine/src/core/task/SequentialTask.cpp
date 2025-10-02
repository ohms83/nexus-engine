#include "core/task/SequentialTask.h"

USING_NAMESPACE_NXS;

SequentialTask::SequentialTask(std::initializer_list<Ref<IRunnable>> taskList)
{
    for (auto task : taskList) {
        m_tasks.push(task);
    }
}

Ref<IRunnable> SequentialTask::Pop()
{
    if (m_tasks.empty()) return nullptr;

    auto task = m_tasks.front();
    m_tasks.pop();
    return task;
}

bool SequentialTask::Update()
{
    if (!m_runningTask && (m_runningTask = Pop()) == nullptr)
    {
        return false;
    }

    if (!m_runningTask->Update()) m_runningTask.reset();

    return true;
}