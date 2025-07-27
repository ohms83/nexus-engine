//
// Created by nutta on 7/25/2025.
//

#pragma once

#include "nexus/NxsDefine.h"
#include <vector>

#include "nexus/scene/SceneRenderer.h"

NXS_NAMESPACE
{
    namespace ECS
    {
        using SimulationSystem = std::function<void(entt::registry&, float)>;

        //! Find all components of the specified type in the given @c registry
        template <typename T>
        std::vector<const T*> FindAllComponents(const entt::registry& registry)
        {
            std::vector<const T*> results;
            for (const auto view = registry.view<T>(); auto [entity, component] : view.each())
            {
                results.push_back(&component);
            }
            return results;
        }
    }
}