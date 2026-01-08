#pragma once

#include "SceneRenderer.h"

NXS_NAMESPACE
{
    class ForwardSceneRenderer final : public SceneRenderer
    {
    public:
        ForwardSceneRenderer(const RenderSystem& renderSystem);
        void Render(RenderSystem& renderSystem, const entt::registry& registry) override;
    };
}