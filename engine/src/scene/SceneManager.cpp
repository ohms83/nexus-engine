#include "scene/SceneManager.h"
#include "core/task/TaskScheduler.h"
#include "core/task/OneshotTask.h"
#include "Engine.h"

USING_NAMESPACE_NXS;

DEFINE_LOG(SceneManager);

SceneManager::SceneManager()
{
}

Ref<Scene> SceneManager::Find(const std::string& name) const
{
    auto itr = std::ranges::find_if(m_sceneList, [name](const Ref<Scene> scene) {
        return scene->GetName() == name;
    });

    if (itr == m_sceneList.end())
    {
        LOG_WARNING(LogSceneManager, std::format("Can't find a scene with name: {}", name));
        return nullptr;
    } 
    return *itr;
}

Ref<Scene> SceneManager::ChangeScene(const std::string& name)
{
    if (auto next = Find(name); next != nullptr)
    {
        return ChangeScene_Internal(next) ? next : nullptr;
    }
    return nullptr;
}

void SceneManager::ChangeSceneDirect(Ref<Scene> scene)
{
    if (std::ranges::find(m_sceneList, scene) == m_sceneList.end()) m_sceneList.push_back(scene);
    ChangeScene_Internal(scene);
}

void SceneManager::Update(float dt)
{
    if (m_current) m_current->Update(dt);
}

void SceneManager::Render(RenderSystem &renderSystem)
{
    if (m_current) m_current->Render(renderSystem);
}

bool SceneManager::ChangeScene_Internal(Ref<Scene> scene)
{    
    if (IsShuttingDown()) return false;

    if (m_next)
    {
        LOG_WARNING(LogSceneManager,
            std::format("There's already scene transitioning in progress. {} => {}",
                m_current->GetName(), m_next->GetName()));
        return false;
    }
    m_next = scene;

    auto taskScheduler = Engine::Instance().GetTaskScheduler();
    // Schedule the scene transitioning task at the begining of the next frame.
    taskScheduler->ScheduleTask(std::make_shared<OneshotTask>([this]() {
        if (m_current) {
            m_current->OnExit();
        }
        m_current = m_next;
        m_current->OnEnter();
        m_next.reset();
    }), TaskScheduler::UpdatePhase::PreUpdate);

    return true;
}