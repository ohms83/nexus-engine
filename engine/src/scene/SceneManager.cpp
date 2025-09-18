#include "scene/SceneManager.h"
#include "core/task/TaskScheduler.h"
#include "core/task/OneshotTask.h"

USING_NAMESPACE_NXS;

DEFINE_LOG(SceneManager);

SceneManager::SceneManager(TaskScheduler& taskScheduler)
    : m_taskScheduler(taskScheduler)
{
}

void SceneManager::Add(Ref<Scene> scene)
{
    NXS_ASSERT(!scene->GetName().empty());
    m_sceneList.push_back(scene);
}

Ref<Scene> SceneManager::ChangeScene(const std::string& name)
{
    if (m_next)
    {
        LOG_WARNING(LogSceneManager,
            std::format("There's already scene transitioning in progress. {} => {}",
                m_current->GetName(), m_next->GetName()));
        return nullptr;
    }

    auto itr = std::ranges::find_if(m_sceneList, [name](const Ref<Scene> scene) {
        return scene->GetName() == name;
    });
    if (itr == m_sceneList.end()) return nullptr;

    m_next = *itr;
    // Schedule the scene transitioning task at the begining of the next frame.
    m_taskScheduler.ScheduleTask(std::make_shared<OneshotTask>([this]() {
        m_current->OnExit();
        m_current = m_next;
        m_current->OnEnter();
        m_next.reset();
    }), TaskScheduler::UpdatePhase::PreUpdate);
    return m_next;
}

void SceneManager::Update()
{
    if (m_current) m_current->Update(0);
}