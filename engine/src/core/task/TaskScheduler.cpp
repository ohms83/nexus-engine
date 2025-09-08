//
// Created by nutta on 9/5/2025.
//
#include "nexus/core/task/TaskScheduler.h"

#include <algorithm>

#include "time/TimerManager.h"
#include "Remotery.h"

static uint32_t s_runningId = 1;

USING_NAMESPACE_NXS;

TaskScheduler::TaskScheduler(const Ref<ITimeSource>& timeSource)
{
    m_timer = std::make_unique<Timer>(timeSource);
}

TaskID TaskScheduler::ScheduleTask(Ref<IRunnable> task, const int32_t repeat, const double delay,
    const double interval, TaskQueue queue)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    const TaskID id = s_runningId++;
    m_tasks.emplace_back(TaskData {
        id,
        delay >= 0,
        repeat,
        delay,
        interval,
        // nextUpdate
        interval,
        std::move(task)
    });
    return id;
}

void TaskScheduler::StopTask(TaskID id)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    const auto taskItr = std::ranges::find_if(m_tasks, [id](const TaskData& task)
    {
        return task.id == id;
    });
    if (taskItr == m_tasks.end()) return;
    m_tasks.erase(taskItr);
}

void TaskScheduler::Update()
{
    rmt_ScopedCPUSample(TaskScheduler_Update, 0);
    std::vector<TaskID> terminatedTasks;
    m_timer->Tick();

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        const auto dt = m_timer->GetDeltaTime();

        std::ranges::for_each(m_tasks, [dt, &terminatedTasks](TaskData& task)
        {
            if (task.delay > 0.0)
            {
                task.delay -= dt;
                task.running = task.delay <= 0.0;
            }

            if (!task.running) return;

            task.nextUpdate -= dt;
            if (task.nextUpdate <= 0.0)
            {
                if (!task.action->Update())
                {
                    terminatedTasks.emplace_back(task.id);
                    return;
                }

                task.nextUpdate = task.interval;
            }
        });
    }

    while (!terminatedTasks.empty())
    {
        StopTask(*terminatedTasks.begin());
        terminatedTasks.erase(terminatedTasks.begin());
    }
}
