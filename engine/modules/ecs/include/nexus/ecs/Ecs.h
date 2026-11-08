//
// Created by nutta on 7/25/2025.
//

#pragma once

#include "nexus/NxsDefine.h"
#include "entt/entt.hpp"
#include "nexus/container/Array.h"

NXS_NAMESPACE
{
    namespace ECS
    {
        using SimulationSystem = std::function<void(entt::registry&, float)>;

        //! Find all components of the specified type in the given @c registry
        template <typename T>
        Array<const T*> FindAllComponents(const entt::registry& registry)
        {
            Array<const T*> results;
            for (const auto view = registry.view<T>(); auto [entity, component] : view.each())
            {
                results.push_back(&component);
            }
            return results;
        }
    }
}