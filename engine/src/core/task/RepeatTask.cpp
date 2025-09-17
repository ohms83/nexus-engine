#include "core/task/RepeatTask.h"

USING_NAMESPACE_NXS;

RepeatTask::RepeatTask(uint32_t numRepeat, Func task)
    : m_numRepeat(numRepeat)
    , m_task(task)
{}

bool RepeatTask::Update()
{
    if (m_numRepeat-- == 0) return false;
    return m_task();
}