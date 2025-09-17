#include "core/task/SequencialTask.h"

USING_NAMESPACE_NXS;

Ref<IRunnable> SequencialTask::Pop()
{
    if (m_tasks.empty()) return nullptr;

    auto task = m_tasks.front();
    m_tasks.pop();
    return task;
}

bool SequencialTask::Update()
{
    if (!m_runningTask && (m_runningTask = Pop()) == nullptr)
    {
        return false;
    }

    if (!m_runningTask->Update()) m_runningTask.reset();

    return true;
}