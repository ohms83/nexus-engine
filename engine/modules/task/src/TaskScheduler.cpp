//
// Created by nutta on 9/5/2025.
//
#include "nexus/task/TaskScheduler.h"

#include <algorithm>

#include "nexus/time/TimerManager.h"
#include "Remotery.h"

USING_NAMESPACE_NXS;

TaskScheduler::TaskScheduler(const Ref<ITimeSource>& timeSource)
{
    for (int i = 0; i < INT_CAST(UpdatePhase::Num); ++i)
    {
        auto& taskGroup = m_taskGroups[CAST<UpdatePhase>(i)];
        taskGroup.taskFinishedCallback.connect([this](Ref<IRunnable> task) {
            OnTaskTerminated(task);
        });
        m_pendingTasks[CAST<UpdatePhase>(i)] = TaskGroup();
    }
}

TaskID TaskScheduler::ScheduleTask(Ref<IRunnable> task, UpdatePhase phase, TaskQueue queue)
{
    if (IsShuttingDown()) return 0;

    std::lock_guard<std::mutex> lock(m_pendingMutex);
    const TaskID id = R_CAST<TaskID>(task.get());
    m_pendingTasks[phase].Add(task);
    return id;
}

void TaskScheduler::StopTask(TaskID id)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    for (auto& keyValue : m_taskGroups)
    {
        keyValue.second.RemoveIf([id](Ref<IRunnable> task) {
            return R_CAST<TaskID>(task.get()) == id;
        });
    }
    
    for (auto& keyValue : m_pendingTasks)
    {
        keyValue.second.RemoveIf([id](Ref<IRunnable> task) {
            return R_CAST<TaskID>(task.get()) == id;
        });
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

void TaskScheduler::TransferPendingTasks()
{
    // Lock both mutexes simultaneously without deadlock risk
    std::scoped_lock lock(m_pendingMutex, m_mutex);
    
    // Safely transfer from m_pendingTasks to m_taskGroups
    for (auto& [phase, tasks] : m_pendingTasks)
    {
        m_taskGroups[phase].Merge(tasks);
        m_pendingTasks[phase].Clear();
    }
}

void TaskScheduler::UpdateTaskGroup(UpdatePhase phase)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_taskGroups[phase].Update();
}

void TaskScheduler::OnTaskTerminated(Ref<IRunnable> task)
{
}

TaskList TaskScheduler::GetAllTasks(bool includePending) const
{
    TaskList tasks;
    for (auto& keyValue : m_taskGroups) {
        tasks.insert(tasks.end(), keyValue.second.GetTasks().begin(), keyValue.second.GetTasks().end());
    }
    if (includePending)
    {
        for (auto& keyValue : m_pendingTasks) {
            tasks.insert(tasks.end(), keyValue.second.GetTasks().begin(), keyValue.second.GetTasks().end());
        }
    }
    return tasks;
}

TaskList TaskScheduler::GetAllTasksFromGroup(UpdatePhase phase, bool includePending) const
{
    TaskList tasks;

    const auto& itr = m_taskGroups.find(phase);
    tasks.insert(tasks.end(), itr->second.GetTasks().begin(), itr->second.GetTasks().end());

    if (includePending)
    {
        const auto& pendingItr = m_pendingTasks.find(phase);
        tasks.insert(tasks.end(), pendingItr->second.GetTasks().begin(), pendingItr->second.GetTasks().end());
    }
    return tasks;
}