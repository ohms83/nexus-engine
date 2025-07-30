//
// Created by nutta on 7/30/2025.
//

#pragma once

#include <functional>
#include <nexus/NxsDefine.h>

#include "Timer.h"

NXS_NAMESPACE
{
    using TaskHandle = uint32;

    class Task
    {
    public:
        using Action = std::function<void()>;

        Task(const Action& action, int32 repeatCount, float delay, float duration);
        virtual ~Task() = default;

    protected:
        TaskHandle m_handle = 0;
        Action m_action;
        Timer m_timer;
        int32 m_repeatCount = 0;
        int32 m_numRepeats = 0;
        float m_delay = 0;
        float m_duration = 0;
    };

    class TaskManager
    {
    public:
    private:
        std::vector<Ptr<Task>> m_tasks;
    };
}
