#pragma once

#include "SceneRenderer.h"

NXS_NAMESPACE
{
    /**
     * @brief A dummy implementation of SceneRenderer that mainly used for testing new features.
     */
    class DummySceneRenderer final : public SceneRenderer
    {
    public:
        DummySceneRenderer();
        ~DummySceneRenderer() override = default;

        void Render(RenderSystem& renderSystem, const entt::registry& registry) override;
    };
}