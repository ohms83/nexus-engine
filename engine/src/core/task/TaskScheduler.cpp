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
    for (auto& keyValue : m_taskGroups)
    {
        keyValue.second.taskFinishedCallback.connect([this](Ref<IRunnable> task) {
            OnTaskTerminated(task);
        });
    }
}

TaskID TaskScheduler::ScheduleTask(Ref<IRunnable> task, UpdatePhase phase, TaskQueue queue)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    const TaskID id = s_runningId++;
    m_tasks.emplace_back(id, task);
    m_taskGroups[phase].Add(task);
    return id;
}

void TaskScheduler::StopTask(TaskID id)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    const auto taskItr = std::ranges::find_if(m_tasks, [id](const TaskData& taskData)
    {
        return taskData.id == id;
    });
    if (taskItr == m_tasks.end()) return;
    m_tasks.erase(taskItr);

    for (auto& keyValue : m_taskGroups)
    {
        keyValue.second.Remove(taskItr->action);
    }
}

void TaskScheduler::PreUpdate()
{
    rmt_ScopedCPUSample(TaskScheduler_PreUpdate, 0);
    UpdateTaskGroup(UpdatePhase::PreUpdate);
}

void TaskScheduler::Update()
{
    rmt_ScopedCPUSample(TaskScheduler_Update, 0);
    UpdateTaskGroup(UpdatePhase::Update);
}

void TaskScheduler::PostUpdate()
{
    rmt_ScopedCPUSample(TaskScheduler_PostUpdate, 0);
    UpdateTaskGroup(UpdatePhase::PostUpdate);
}

void TaskScheduler::UpdateTaskGroup(UpdatePhase phase)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_taskGroups[phase].Update();
}

void TaskScheduler::OnTaskTerminated(Ref<IRunnable> task)
{
    const auto taskItr = std::ranges::find_if(m_tasks, [task](const TaskData& taskData)
    {
        return taskData.action == task;
    });
    if (taskItr == m_tasks.end()) return;
    m_tasks.erase(taskItr);
}