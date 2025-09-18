#include "core/task/RepeatTask.h"

USING_NAMESPACE_NXS;

bool RepeatTask::Update()
{
    if (m_numRepeat >= 0 && m_numRepeat-- == 0) return false;
    return m_task();
}