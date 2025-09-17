#include "scene/SceneManager.h"

USING_NAMESPACE_NXS;

void SceneManager::Add(const std::string& name, Ref<Scene> scene)
{

}

Ref<Scene> SceneManager::ChangeScene(const std::string& name)
{
    return nullptr;
}

void SceneManager::Update()
{
    if (m_next)
    {
        m_current->OnExit();
        m_current = m_next;
        m_current->OnEnter();
        m_next.reset();
    }
    if (m_current) m_current->Update(0);
}