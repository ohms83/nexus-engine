#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/core/task/TaskScheduler.h"
#include "Scene.h"

#include <vector>
#include <concepts>

NXS_NAMESPACE
{
    class SceneManager
    {
    public:
        SceneManager(TaskScheduler& taskScheduler);

        void Add(Ref<Scene> scene);

        template<typename SceneType, typename... Args>
        requires std::derived_from<SceneType, SceneType>
        void Emplace(const std::string& name, Args &&...args)
        {
            m_sceneList.push_back(std::make_shared<SceneType>(name, std::forward(args)...));
        }

        Ref<Scene> ChangeScene(const std::string& name);

        void Update();

    private:
        std::vector<Ref<Scene>> m_sceneList;
        Ref<Scene> m_next;
        Ref<Scene> m_current;
        TaskScheduler& m_taskScheduler;
    };
}