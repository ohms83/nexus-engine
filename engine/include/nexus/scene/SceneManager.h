#pragma once

#include "nexus/NxsDefine.h"
#include "nexus/graphics/RenderSystem.h"
#include "nexus/core/task/TaskScheduler.h"
#include "Scene.h"

#include <vector>
#include <concepts>

NXS_NAMESPACE
{
    class SceneManager
    {
    public:
        SceneManager(Ref<TaskScheduler> taskScheduler);

        template<typename SceneType, typename... Args>
        requires std::derived_from<SceneType, SceneType>
        Ref<SceneType> Emplace(const std::string& name, Args &&...args)
        {
            NXS_ASSERT(!name.empty());
            auto scene = std::make_shared<SceneType>(name, std::forward(args)...);
            scene->SetTaskScheduler(m_taskScheduler);
            m_sceneList.push_back(scene);
            return scene;
        }

        template<typename SceneType, typename... Args>
        requires std::derived_from<SceneType, SceneType>
        Ref<SceneType> EmplaceAndChange(const std::string& name, Args &&...args)
        {
            auto scene = Emplace<SceneType>(name, std::forward(args)...);
            ChangeScene_Internal(scene);
            return scene;
        }
        
        Ref<Scene> Find(const std::string& name) const;
        /**
         * @brief Find the scene with the specified name begin transitioning.
         * 
         * @param name Scene name
         * @return Ref<Scene> The target scene or @c nullptr , if not exists.
         */
        Ref<Scene> ChangeScene(const std::string& name);
        /**
         * @brief Directly change to the specified scene. The scene will be added if it doesn't already exist.
         * 
         * @param scene Target scene
         */
        void ChangeSceneDirect(Ref<Scene> scene);

        Ref<Scene> GetCurrentScene() const { return m_current; }
        Ref<Scene> GetNextScene() const { return m_next; }

        void Update(float dt);
        void Render(RenderSystem &renderSystem);

    private:
        bool ChangeScene_Internal(Ref<Scene> scene);

        std::vector<Ref<Scene>> m_sceneList;
        Ref<Scene> m_next;
        Ref<Scene> m_current;
        Ref<TaskScheduler> m_taskScheduler;
    };
}