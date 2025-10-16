//
// Created by nutta on 7/25/2025.
//

#pragma once

#include "entt/entity/registry.hpp"
#include "nexus/NxsDefine.h"

NXS_NAMESPACE
{
    class RenderSystem;
    class Scene;
    struct LightComponent;
    struct PointLightComponent;
    struct DirectLightComponent;

    class ISceneRenderer
    {
    public:
        virtual ~ISceneRenderer() = default;
        virtual void Render(RenderSystem& renderSystem, const entt::registry& registry) = 0;
    };

    class BasicSceneRenderer final :public ISceneRenderer
    {
    public:
        void Render(RenderSystem& renderSystem, const entt::registry& registry) override;
    };
}
