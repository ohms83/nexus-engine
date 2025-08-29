//
// Created by nutta on 7/30/2025.
//

#include "nexus/core/TaskManager.h"

#include <algorithm>
#include <mutex>

#include "nexus/core/Hasher.h"
#include "nexus/core/LogDispatcher.h"

USING_NAMESPACE_NXS;

static Ptr<TaskManager> s_taskManager;
static std::mutex s_mutex;

void TaskManager::Destroy()
{
    std::lock_guard<std::mutex> lock(s_mutex);
    s_taskManager.reset();
}

void TaskManager::Init()
{
    std::lock_guard<std::mutex> lock(s_mutex);
    s_taskManager.reset(new TaskManager());
}

TaskManager& TaskManager::Instance()
{
    NXS_ASSERT_MSG(s_taskManager, "TaskManager::Instance is not initialized.");
    std::lock_guard<std::mutex> lock(s_mutex);
    return *s_taskManager;
}

TaskHandle TaskManager::CreateTask(const Action& action, int32 repeatCount, float delay, float interval, bool runImmediate)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    const uint64 data = R_CAST<uint64>(&action);
    const TaskHandle handle = Hasher().Hash32(&data, sizeof data);
    m_tasks.emplace_back(TaskInfo {
        .handle = handle,
        .task = std::make_unique<Task>(action, repeatCount, delay, interval, runImmediate)
    });
    return handle;
}

void TaskManager::StopTask(TaskHandle taskHandle)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    const auto itr = std::ranges::find_if(m_tasks, [taskHandle](const TaskInfo& info)
    {
        return info.handle == taskHandle;
    });
    if (itr != m_tasks.end())
    {
        itr->task->Stop();
        m_tasks.erase(itr);
    }
}

void TaskManager::Update()
{
    std::ranges::for_each(m_tasks, [](const TaskInfo& info)
    {
        info.task->Update();
    });
}
